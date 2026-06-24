#include "include/Game/Objects/ManaFind.hpp"

#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"
#include "include/AudioManager/AudioManager.hpp"

ManaFind::ManaFind() : PickUpAbstract() {}

ManaFind::ManaFind(const unordered_map<string, std::any>& data) : PickUpAbstract(data) {
    loopSoundName = "pickable_loop";
}

ManaFind::~ManaFind() {}

void ManaFind::OnPickUp() {
    auto& globals = Globals::GetGlobals();
    auto cardManager = globals.cardManager;

    if (cardManager) {
        player->SetRespawnPoint(GetTransform().GetTranslation(), globals.activeLevelName);
		cardManager->AddMaxMana(1);
    }
    if (auto aum = Globals::GetGlobals().audioManager) {
        aum->PlaySound3D("mana_pickup", GetTransform().GetTranslation(), 1.5f, 1.0f);
    }
    if (audio) {
        audio->Stop();
    }
}
