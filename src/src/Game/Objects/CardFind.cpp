#include "include/Game/Objects/CardFind.hpp"

#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"
#include "include/AudioManager/AudioManager.hpp"

CardFind::CardFind() : PickUpAbstract(){}

CardFind::CardFind(const unordered_map<string, std::any>& data) : PickUpAbstract(data) {
    int cardType = (int)fromMap(int64_t, "cardType", data);
    cardTypeToUnlock = static_cast<CardType>(cardType);

    color2 = vec3(fromMap(float, "lightR", data)/255, fromMap(float, "lightG", data)/255, fromMap(float, "lightB", data)/255);
	color1 = color2 * 0.5f;
	specular1 = color1 * 0.05f;
	specular2 = color2 * 0.05f;
    loopSoundName = "pickable_loop";
}

CardFind::~CardFind() {}

void CardFind::OnPickUp() {
    auto& globals = Globals::GetGlobals();
    auto cardManager = globals.cardManager;

    if (cardManager) {
        std::shared_ptr<Card> newCard = cardManager->CreateCard(cardTypeToUnlock);
        newCard->AssignPlayer(player);
        cardManager->ReachCheckpoint();
        player->SetRespawnPoint(GetTransform().GetTranslation(), globals.activeLevelName);
        cardManager->LearnCard(newCard);
    }
    if (auto aum = Globals::GetGlobals().audioManager) {
        aum->PlaySound3D("player_spotted", GetTransform().GetTranslation(), 0.5f, 1.0f);
    }
    if (audio) {
        audio->Stop();
    }
}
