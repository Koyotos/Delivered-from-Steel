#include "include/AudioManager/AudioManager.hpp"
#include "include/Globals/Globals.hpp"
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <iostream>
#include <fstream>

AudioManager::AudioManager() : device(nullptr), context(nullptr) {}

AudioManager::~AudioManager() {
	CleanUp();
}

bool AudioManager::Init() {
	device = alcOpenDevice(nullptr);
	if (!device) {
		Globals::GetGlobals().Log("AUDIO ERROR: Can't open OpenAL device.");
		return false;
	}

	context = alcCreateContext(device, nullptr);
	if (!context || !alcMakeContextCurrent(context)) {
		Globals::GetGlobals().Log("AUDIO ERROR: Can't open OpenAL context.");
		if (context) alcDestroyContext(context);
		alcCloseDevice(device);
		return false;
	}

	audioSources.resize(MAX_SOURCES);
	alGenSources(MAX_SOURCES, audioSources.data());

	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	Globals::GetGlobals().Log("Audio Manager initialized.");
	return true;
}

void AudioManager::CleanUp() {
	StopAll();
	StopAllBGM();

	if (!audioSources.empty() && context) {
		alDeleteSources(audioSources.size(), audioSources.data());
		audioSources.clear();
	}
	if (context) {
		for (auto& pair : audioBuffers) {
			alDeleteBuffers(1, &pair.second);
		}
	}
	audioBuffers.clear();

	if (context) {
		for (auto& pair : streams) {
			alDeleteSources(1, &pair.second.source);
			alDeleteBuffers(4, pair.second.buffers);
			if (pair.second.oggStream) {
				stb_vorbis_close(pair.second.oggStream);
			}
		}
	}
	streams.clear();

	if (context) {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		context = nullptr;
	}
	if (device) {
		alcCloseDevice(device);
		device = nullptr;
	}
}

void AudioManager::Update() {
	if (!context) return;
	bool isAnyStreamPlaying = false;

	for (auto& pair : streams) {
		AudioStream& stream = pair.second;
		if (!stream.isPlaying) continue;

		ALint processed;
		alGetSourcei(stream.source, AL_BUFFERS_PROCESSED, &processed);

		while (processed--) {
			ALuint buffer;
			alSourceUnqueueBuffers(stream.source, 1, &buffer);

			if (stream.oggStream) {
				if (StreamBufferData(buffer, stream)) {
					alSourceQueueBuffers(stream.source, 1, &buffer);
				}
				else {
					stb_vorbis_close(stream.oggStream);
					stream.oggStream = nullptr;
				}
			}
		}

		ALint queued;
		alGetSourcei(stream.source, AL_BUFFERS_QUEUED, &queued);
		ALint state;
		alGetSourcei(stream.source, AL_SOURCE_STATE, &state);

		if (queued == 0) {
			stream.isPlaying = false;
			continue;
		}

		if (queued > 0 && state != AL_PLAYING) {
			alSourcePlay(stream.source);
		}

		isAnyStreamPlaying = true;
	}

	if (isPlaylistActive && !isAnyStreamPlaying) {
		currentPlaylistIndex++;
		if (currentPlaylistIndex >= currentPlaylist.size()) {
			currentPlaylistIndex = 0;
		}
		PlayBGM(currentPlaylist[currentPlaylistIndex], playlistVolume, false);
		isPlaylistActive = true;
	}
}

ALuint AudioManager::LoadWav(const std::string& filepath) {
	if (!context) return 0;

	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open()) {
		Globals::GetGlobals().Log("AUDIO ERROR: Can't find " + filepath);
		return 0;
	}

	char buffer[4];
	file.read(buffer, 4);
	file.seekg(4, std::ios::cur);
	file.read(buffer, 4);
	file.read(buffer, 4);

	uint32_t fmtSize;
	file.read(reinterpret_cast<char*>(&fmtSize), 4);

	uint16_t audioFormat, numChannels;
	uint32_t sampleRate, byteRate;
	uint16_t blockAlign, bitsPerSample;

	file.read(reinterpret_cast<char*>(&audioFormat), 2);
	file.read(reinterpret_cast<char*>(&numChannels), 2);
	file.read(reinterpret_cast<char*>(&sampleRate), 4);
	file.read(reinterpret_cast<char*>(&byteRate), 4);
	file.read(reinterpret_cast<char*>(&blockAlign), 2);
	file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

	if (fmtSize > 16) file.seekg(fmtSize - 16, std::ios::cur);

	uint32_t dataSize;
	while (true) {
		file.read(buffer, 4);
		file.read(reinterpret_cast<char*>(&dataSize), 4);
		if (std::string(buffer, 4) == "data") break;
		file.seekg(dataSize, std::ios::cur);
		if (file.eof()) return 0;
	}

	std::vector<char> audioData(dataSize);
	file.read(audioData.data(), dataSize);

	ALenum format = 0;
	if (numChannels == 1 && bitsPerSample == 8) format = AL_FORMAT_MONO8;
	else if (numChannels == 1 && bitsPerSample == 16) format = AL_FORMAT_MONO16;
	else if (numChannels == 2 && bitsPerSample == 8) format = AL_FORMAT_STEREO8;
	else if (numChannels == 2 && bitsPerSample == 16) format = AL_FORMAT_STEREO16;

	if (format == 0) {
		Globals::GetGlobals().Log("AUDIO ERROR: Unknown format " + filepath);
		return 0;
	}

	ALuint alBuffer;
	alGenBuffers(1, &alBuffer);
	alBufferData(alBuffer, format, audioData.data(), dataSize, sampleRate);

	return alBuffer;
}

void AudioManager::LoadSound(const std::string& name, const std::string& filepath) {
	if (!context) return;
	if (audioBuffers.find(name) != audioBuffers.end()) return;

	ALuint buffer = LoadWav(filepath);
	if (buffer != 0) {
		audioBuffers[name] = buffer;
	}
}

ALuint AudioManager::GetAvailableSource() {
	if (!context) return 0;
	for (ALuint source : audioSources) {
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING && state != AL_PAUSED) {
			return source;
		}
	}
	return 0;
}

void AudioManager::PlaySound2D(const std::string& name, float volume, float pitch, bool loop) {
	if (!context) return;
	auto it = audioBuffers.find(name);
	if (it == audioBuffers.end()) return;

	ALuint source = GetAvailableSource();
	if (source == 0) return;

	alSourcei(source, AL_BUFFER, it->second);
	alSourcef(source, AL_PITCH, pitch);
	alSourcef(source, AL_GAIN, volume);
	alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);

	alSourcePlay(source);
}

void AudioManager::PlaySound3D(const std::string& name, glm::vec3 position, float volume, float pitch, bool loop) {
	if (!context) return;
	auto it = audioBuffers.find(name);
	if (it == audioBuffers.end()) return;

	ALuint source = GetAvailableSource();
	if (source == 0) return;

	alSourcei(source, AL_BUFFER, it->second);
	alSourcef(source, AL_PITCH, pitch);
	alSourcef(source, AL_GAIN, volume);
	alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);

	alSourcef(source, AL_REFERENCE_DISTANCE, 5.0f);
	alSourcef(source, AL_MAX_DISTANCE, 50.0f);

	alSourcePlay(source);
}


void AudioManager::RegisterBGM(const std::string& name, const std::string& filepath) {
	if (!context) return;
	if (streams.find(name) != streams.end()) return;

	AudioStream newStream;
	alGenSources(1, &newStream.source);
	alGenBuffers(4, newStream.buffers);
	newStream.currentFile = filepath;
	newStream.isPlaying = false;

	streams[name] = newStream;
}

bool AudioManager::StreamBufferData(ALuint buffer, AudioStream& stream) {
	if (!context || !stream.oggStream) return false;

	thread_local std::vector<short> pcm;
	if (pcm.size() != BUFFER_SIZE) {
		pcm.resize(BUFFER_SIZE);
	}
	const int channels = (stream.format == AL_FORMAT_STEREO16 ? 2 : 1);

	for (int attempt = 0; attempt < (stream.loop ? 2 : 1); ++attempt) {
		int samples = stb_vorbis_get_samples_short_interleaved(
			stream.oggStream, channels, pcm.data(), BUFFER_SIZE
		);

		if (samples > 0) {
			int size = samples * channels * sizeof(short);
			alBufferData(buffer, stream.format, pcm.data(), size, stream.sampleRate);
			return true;
		}

		if (!stream.loop || attempt > 0) {
			break;
		}

		stb_vorbis_seek_start(stream.oggStream);
	}

	return false;
}

void AudioManager::PlayBGM(const std::string& name, float volume, bool loop) {
	if (!context) return;
	auto it = streams.find(name);
	if (it == streams.end()) return;

	AudioStream& stream = it->second;
	if (stream.isPlaying) return;

	StopAllBGM();

	int error;
	stream.oggStream = stb_vorbis_open_filename(stream.currentFile.c_str(), &error, nullptr);
	if (!stream.oggStream) {
		Globals::GetGlobals().Log("AUDIO ERROR: Can't open ogg file: " + stream.currentFile);
		return;
	}

	stb_vorbis_info info = stb_vorbis_get_info(stream.oggStream);
	stream.format = (info.channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	stream.sampleRate = info.sample_rate;
	stream.loop = loop;

	ALuint queuedBuffers[4];
	int filledBufferCount = 0;
	for (int i = 0; i < 4; ++i) {
		if (StreamBufferData(stream.buffers[i], stream)) {
			queuedBuffers[filledBufferCount++] = stream.buffers[i];
		}
	}

	if (filledBufferCount == 0) {
		stb_vorbis_close(stream.oggStream);
		stream.oggStream = nullptr;
		stream.isPlaying = false;
		return;
	}

	alSourceQueueBuffers(stream.source, filledBufferCount, queuedBuffers);
	alSourcef(stream.source, AL_GAIN, volume);
	alSourcei(stream.source, AL_SOURCE_RELATIVE, AL_TRUE);

	alSourcePlay(stream.source);
	stream.isPlaying = true;
}


void AudioManager::StopBGM(const std::string& name) {
	if (!context) return;
	auto it = streams.find(name);
	if (it == streams.end()) return;

	AudioStream& stream = it->second;
	alSourceStop(stream.source);

	ALint queued;
	alGetSourcei(stream.source, AL_BUFFERS_QUEUED, &queued);
	while (queued--) {
		ALuint buffer;
		alSourceUnqueueBuffers(stream.source, 1, &buffer);
	}

	if (stream.oggStream) {
		stb_vorbis_close(stream.oggStream);
		stream.oggStream = nullptr;
	}
	stream.isPlaying = false;
}

void AudioManager::StopAllBGM() {
	isPlaylistActive = false;
	for (auto& pair : streams) {
		StopBGM(pair.first);
	}
}

void AudioManager::SetListenerPosition(glm::vec3 position) {
	if (!context) return;
	alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void AudioManager::StopAll() {
	if (!context) return;
	for (ALuint source : audioSources) {
		alSourceStop(source);
	}
}

void AudioManager::PlayPlaylist(const std::vector<std::string>& trackNames, float volume) {
	if (!context || trackNames.empty()) return;

	currentPlaylist = trackNames;
	currentPlaylistIndex = 0;
	isPlaylistActive = true;
	playlistVolume = volume;

	PlayBGM(currentPlaylist[currentPlaylistIndex], volume, false);
	isPlaylistActive = true;
}
