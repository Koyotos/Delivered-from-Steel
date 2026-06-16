#ifndef FE_AUDIO_SOURCE
#define FE_AUDIO_SOURCE

#include <string>
#include <cstdint>

class VisualNode;
typedef uint32_t AudioHandle;

class AudioSource {
private:
	AudioHandle handle = 0;
	std::string loopingSoundName = "";
	VisualNode* owner;

public:
	AudioSource(VisualNode* ownerNode);
	~AudioSource();

	void PlayLooping(const std::string& name, float volume = 1.0f, float pitch = 1.0f, float maxDistance = 10.0f, float refDistance = 1.0f);
	void Update();
	void Stop();
};

#endif