#include "include/AudioManager/AudioSource.hpp"
#include "include/AudioManager/AudioManager.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Core/VisualNode.hpp"

AudioSource::AudioSource(VisualNode* ownerNode) : owner(ownerNode) {}

AudioSource::~AudioSource() {
	Stop();
}

void AudioSource::PlayLooping(const std::string& name, float volume, float pitch, float maxDistance, float refDistance) {
	if (loopingSoundName == name && handle != 0) return;

	if (auto aum = Globals::GetGlobals().audioManager) {
		if (handle != 0) {
			aum->StopSound3D(handle);
		}
		handle = aum->PlaySound3D(name, owner->GetTransform().GetTranslation(), volume, pitch, true, maxDistance, refDistance);
		loopingSoundName = name;
	}
}

void AudioSource::Update() {
	if (handle != 0) {
		if (auto aum = Globals::GetGlobals().audioManager) {
			aum->UpdateSound3DPosition(handle, owner->GetTransform().GetTranslation());
		}
	}
}

void AudioSource::Stop() {
	if (handle != 0) {
		if (auto aum = Globals::GetGlobals().audioManager) {
			aum->StopSound3D(handle);
		}
		handle = 0;
		loopingSoundName = "";
	}
}