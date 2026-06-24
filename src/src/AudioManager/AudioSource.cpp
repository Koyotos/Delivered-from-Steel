#include "include/AudioManager/AudioSource.hpp"
#include "include/AudioManager/AudioManager.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Core/VisualNode.hpp"

AudioSource::AudioSource(VisualNode* ownerNode) : owner(ownerNode) {}

AudioSource::~AudioSource() {
	Stop();
}

void AudioSource::PlayLooping(const std::string& name, float volume, float pitch, float maxDistance, float refDistance) {
	if (auto aum = Globals::GetGlobals().audioManager) {
		if (handle != 0 && !aum->IsSoundActive(handle)) {
			handle = 0;
		}

		float distance = glm::distance(owner->GetTransform().GetTranslation(), aum->GetListenerPosition());
		if (distance > maxDistance + 2.0f) {
			if (handle != 0) {
				Stop();
			}
			return;
		}
		if (loopingSoundName == name && handle != 0) return;

		if (handle != 0) {
			Stop();
		}
		handle = aum->PlaySound3D(name, owner->GetTransform().GetTranslation(), volume, pitch, true, maxDistance, refDistance);
		loopingSoundName = name;
		is2D = false;
	}
}

void AudioSource::PlayLooping2D(const std::string& name, float volume, float pitch) {
	if (auto aum = Globals::GetGlobals().audioManager) {
		if (handle != 0 && !aum->IsSoundActive(handle)) {
			handle = 0;
		}

		if (loopingSoundName == name && handle != 0) {
			aum->UpdateSoundParams(handle, volume, pitch);
			return;
		}

		if (handle != 0) {
			Stop();
		}
		handle = aum->PlaySound2D(name, volume, pitch, true);
		loopingSoundName = name;
		is2D = true;
	}
}

void AudioSource::Update() {
	if (handle != 0 && !is2D) {
		if (auto aum = Globals::GetGlobals().audioManager) {
			if (!aum->IsSoundActive(handle)) {
				handle = 0;
				return;
			}
			aum->UpdateSound3DPosition(handle, owner->GetTransform().GetTranslation());
		}
	}
}

void AudioSource::Stop() {
	if (handle != 0) {
		if (auto aum = Globals::GetGlobals().audioManager) {
			if (aum->IsSoundActive(handle)) {
				aum->StopSound(handle);
			}
		}
		handle = 0;
		loopingSoundName = "";
	}
}