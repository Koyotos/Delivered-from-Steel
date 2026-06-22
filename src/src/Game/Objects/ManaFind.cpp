#include "include/Game/Objects/ManaFind.hpp"

#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

ManaFind::ManaFind() : PickUpAbstract() {}

ManaFind::ManaFind(const unordered_map<string, std::any>& data) : PickUpAbstract(data) {

}

ManaFind::~ManaFind() {}

void ManaFind::OnPickUp() {
    auto& globals = Globals::GetGlobals();
    auto cardManager = globals.cardManager;

    if (cardManager) {
        player->SetRespawnPoint(GetTransform().GetTranslation(), globals.activeLevelName);
		// cardManager-> funkjca dodaj¹ca manê
    }
}
