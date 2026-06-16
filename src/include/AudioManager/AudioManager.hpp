#ifndef FE_AUDIO_MANAGER
#define FE_AUDIO_MANAGER

#include "include/Globals/Globals.hpp"
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <fstream>

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/vec3.hpp>
#include <AL/al.h>
#include <AL/alc.h>

using namespace std;
using namespace glm;

struct stb_vorbis;

typedef uint32_t AudioHandle;

/**
 * @brief Structure managing a single streaming audio channel (BGM).
 */
struct AudioStream {
	ALuint source;
	ALuint buffers[4];
	stb_vorbis* oggStream = nullptr;
	ALenum format;
	ALuint sampleRate;
	bool isPlaying = false;
	bool loop = true;
	string currentFile = "";
	bool isAmbient = false;
	float baseVolume = 1.0f;
	int fadeState = 0; // 0 = no fade, 1 = fading in, -1 = fading out
	float currentFadeMultiplier = 1.0f;
	float fadeSpeed = 1.0f;
};

/**
 * @brief Module responsible for audio playback using OpenAL.
 * Supports RAM-loaded SFX (.wav) and streamed BGM (.ogg).
 */
class AudioManager {
private:
	ALCdevice* device;
	ALCcontext* context;

	unordered_map<string, ALuint> audioBuffers;
	vector<ALuint> audioSources;
	static constexpr int MAX_SOURCES = 64;

	struct ActiveSound {
		ALuint source;
		string name;
	};
	unordered_map<AudioHandle, ActiveSound> activeSounds;
	AudioHandle nextHandleId = 1;

	unordered_map<string, AudioStream> streams;
	static constexpr int BUFFER_SIZE = 32768;

	vector<string> currentPlaylist;
	int currentPlaylistIndex = 0;
	bool isPlaylistActive = false;
	float playlistVolume = 1.0f;

	float masterVolume = 1.0f;
	float sfxVolume = 1.0f;
	float bgmVolume = 1.0f;
	float ambientVolume = 1.0f;

	/**
	 * @brief Loads a .wav file from disk to a buffer.
	 * @param1 const std::string& - filepath
	 * @return ALuint - generated buffer ID
	 */
	ALuint LoadWav(const string& filepath);
	/**
	 * @brief Loads an .ogg file from disk to a buffer.
	 * @param1 const std::string& - filepath
	 * @return ALuint - generated buffer ID
	 */
	ALuint LoadOgg(const string& filepath);
	/**
	 * @brief Finds an idle source from the pool.
	 * @return ALuint - source ID
	 */
	ALuint GetAvailableSource();
	/**
	 * @brief Refills a streaming buffer with new OGG data.
	 * @param1 ALuint - buffer to fill
	 * @param2 AudioStream& - stream state
	 * @return bool - true if data was read
	 */
	bool StreamBufferData(ALuint buffer, AudioStream& stream);
	bool StartStream(const string& name, float volume, bool loop);

public:
	AudioManager();
	~AudioManager();

	/**
	 * @brief Initializes OpenAL device and context.
	 * @return bool - true if successful
	 */
	bool Init();
	/**
	 * @brief Stops all sounds and releases OpenAL resources.
	 * @return void
	 */
	void CleanUp();
	/**
	 * @brief Updates streaming buffers. Must be called every frame.
	 * @param1 float - delta time since last update
	 * @return void
	 */
	void Update(float deltaTime);
	/**
	 * @brief Loads a sound effect into memory.
	 * @param1 const std::string& - unique name for the sound
	 * @param2 const std::string& - path to .wav file
	 * @return void
	 */
	void LoadSound(const string& name, const string& filepath);
	/**
	 * @brief Plays a 2D sound effect (no spatial attenuation).
	 * @param1 const std::string& - sound name
	 * @param2 float - volume (0.0 to 1.0)
	 * @param3 float - pitch (1.0 default)
	 * @param4 bool - should loop
	 * @return AudioHandle - handle to the active sound, or 0 if failed
	 */
	AudioHandle PlaySound2D(const string& name, float volume = 1.0f, float pitch = 1.0f, bool loop = false);
	/**
	 * @brief Plays a 3D sound effect with spatial attenuation.
	 * @param1 const std::string& - sound name
	 * @param2 glm::vec3 - world position
	 * @param3 float - volume
	 * @param4 float - pitch
	 * @param5 bool - should loop
	 * @param6 float - max distance for attenuation
	 * @param7 float - reference distance for attenuation
	 * @return AudioHandle - handle to the active 3D sound
	 */
	AudioHandle PlaySound3D(const string& name, vec3 position, float volume = 1.0f, float pitch = 1.0f, bool loop = false, float maxDistance = 6.0f, float refDistance = 1.0f);
	void UpdateSound3DPosition(AudioHandle handle, vec3 newPosition);
	void StopSound(AudioHandle handle);

	/**
	 * @brief Registers an OGG file for streaming.
	 * @param1 const std::string& - stream name
	 * @param2 const std::string& - path to .ogg file
	 * @return void
	 */
	void RegisterBGM(const string& name, const string& filepath);
	/**
	 * @brief Starts playing a registered BGM stream.
	 * @param1 const std::string& - stream name
	 * @param2 float - volume
	 * @param3 bool - should loop
	 * @return true if playback started successfully
	 */
	bool PlayBGM(const string& name, float volume = 1.0f, bool loop = true);
	/**
	 * @brief Stops a specific stream.
	 * @param1 const std::string& - stream name
	 * @return void
	 */
	void StopStream(const string& name);
	/**
	 * @brief Stops all active BGM streams.
	 * @return void
	 */
	void StopAllBGM();
	/**
	 * @brief Updates listener's ears position in the world.
	 * @param1 glm::vec3 - position
	 * @return void
	 */
	void SetListenerPosition(vec3 position);
	/**
	 * @brief Stops all SFX sources.
	 * @return void
	 */
	void StopAll();
	/**
	* @brief Plays a sequence of registered BGM tracks as a playlist. Automatically advances to the next track when one finishes. Loops the playlist.
	* @param1 const std::vector<std::string>& - list of registered BGM stream names
	* @param2 float - volume
	*/
	void PlayPlaylist(const vector<string>& trackNames, float volume = 1.0f);

	void SetMasterVolume(float volume);
	void SetSFXVolume(float volume);
	void SetBGMVolume(float volume);
	void SetAmbientVolume(float volume);

	void RegisterAmbient(const string& name, const string& filepath);
	bool PlayAmbient(const string& name, float volume = 1.0f, bool loop = true);
	void StopAllAmbient();

	void FadeOutAllBGM(float duration = 1.5f);
	void FadeOutAllAmbient(float duration = 1.5f);
};

#endif