#include "include/Game/Objects/CardFind.hpp"

#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

CardFind::CardFind() : PickUpAbstract(){}

CardFind::CardFind(const unordered_map<string, std::any>& data) : PickUpAbstract(data) {
    int cardType = (int)fromMap(int64_t, "cardType", data);
    cardTypeToUnlock = static_cast<CardType>(cardType);

    color2 = vec3(fromMap(float, "lightR", data)/255, fromMap(float, "lightG", data)/255, fromMap(float, "lightB", data)/255);
	color1 = color2 * 0.5f;
	specular1 = color1 * 0.05f;
	specular2 = color2 * 0.05f;
}

CardFind::~CardFind() {}

void CardFind::OnPickUp() {
    auto cardManager = Globals::GetGlobals().cardManager;

    if (cardManager) {
        std::shared_ptr<Card> newCard = cardManager->CreateCard(cardTypeToUnlock);
        newCard->AssignPlayer(player);
        cardManager->LearnCard(newCard);
    }
}
