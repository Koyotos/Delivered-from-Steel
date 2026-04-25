#ifndef FE_AUDIO_MANAGER
#define FE_AUDIO_MANAGER

#include <string>
#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>

#include <AL/al.h>
#include <AL/alc.h>

struct stb_vorbis;

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
	std::string currentFile = "";
};

/**
 * @brief Module responsible for audio playback using OpenAL.
 * Supports RAM-loaded SFX (.wav) and streamed BGM (.ogg).
 */
class AudioManager {
private:
	ALCdevice* device;
	ALCcontext* context;

	std::unordered_map<std::string, ALuint> audioBuffers;
	std::vector<ALuint> audioSources;
	const int MAX_SOURCES = 32;

	std::unordered_map<std::string, AudioStream> streams;
	const int BUFFER_SIZE = 32768;

	std::vector<std::string> currentPlaylist;
	int currentPlaylistIndex = 0;
	bool isPlaylistActive = false;
	float playlistVolume = 1.0f;

	/**
	 * @brief Loads a .wav file from disk to a buffer.
	 * @param1 const std::string& - filepath
	 * @return ALuint - generated buffer ID
	 */
	ALuint LoadWav(const std::string& filepath);
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
	 * @return void
	 */
	void Update();
	/**
	 * @brief Loads a sound effect into memory.
	 * @param1 const std::string& - unique name for the sound
	 * @param2 const std::string& - path to .wav file
	 * @return void
	 */
	void LoadSound(const std::string& name, const std::string& filepath);
	/**
	 * @brief Plays a 2D sound effect (no spatial attenuation).
	 * @param1 const std::string& - sound name
	 * @param2 float - volume (0.0 to 1.0)
	 * @param3 float - pitch (1.0 default)
	 * @param4 bool - should loop
	 * @return void
	 */
	void PlaySound2D(const std::string& name, float volume = 1.0f, float pitch = 1.0f, bool loop = false);
	/**
	 * @brief Plays a 3D sound effect with spatial attenuation.
	 * @param1 const std::string& - sound name
	 * @param2 glm::vec3 - world position
	 * @param3 float - volume
	 * @param4 float - pitch
	 * @param5 bool - should loop
	 * @return void
	 */
	void PlaySound3D(const std::string& name, glm::vec3 position, float volume = 1.0f, float pitch = 1.0f, bool loop = false);

	/**
	 * @brief Registers an OGG file for streaming.
	 * @param1 const std::string& - stream name
	 * @param2 const std::string& - path to .ogg file
	 * @return void
	 */
	void RegisterBGM(const std::string& name, const std::string& filepath);
	/**
	 * @brief Starts playing a registered BGM stream.
	 * @param1 const std::string& - stream name
	 * @param2 float - volume
	 * @param3 bool - should loop
	 * @return void
	 */
	void PlayBGM(const std::string& name, float volume = 1.0f, bool loop = true);
	/**
	 * @brief Stops a specific BGM stream.
	 * @param1 const std::string& - stream name
	 * @return void
	 */
	void StopBGM(const std::string& name);
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
	void SetListenerPosition(glm::vec3 position);
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
	void PlayPlaylist(const std::vector<std::string>& trackNames, float volume = 1.0f);
};

#endif