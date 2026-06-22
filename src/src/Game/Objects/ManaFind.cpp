#include "include/Game/Objects/ManaFind.hpp"

#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"
#include "include/AudioManager/AudioManager.hpp"

ManaFind::ManaFind() : PickUpAbstract() {}

ManaFind::ManaFind(const unordered_map<string, std::any>& data) : PickUpAbstract(data) {
    loopSoundName = "charger_engine";
}

ManaFind::~ManaFind() {}

void ManaFind::OnPickUp() {
    auto cardManager = Globals::GetGlobals().cardManager;

    if (cardManager) {
		// cardManager-> funkjca dodaj¹ca manê
    }
    if (auto aum = Globals::GetGlobals().audioManager) {
        aum->PlaySound3D("player_spotted", GetTransform().GetTranslation(), 0.5f, 1.0f);
    }
    if (audio) {
        audio->Stop();
    }
}
