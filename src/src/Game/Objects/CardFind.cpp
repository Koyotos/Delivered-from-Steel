#include "include/Game/Objects/CardFind.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

CardFind::CardFind() : Object2D(), isCollected(false) {}

CardFind::CardFind(const unordered_map<string, std::any>& data) : Object2D(data), isCollected(false) {
    int cardType = (int)fromMap(int64_t, "cardType", data);
    cardTypeToUnlock = static_cast<CardType>(cardType);
}

CardFind::~CardFind() {}

void CardFind::OnCollisionEnter(std::shared_ptr<Collider> other) {
    if (isCollected) return;

    std::shared_ptr<PhysicsNode> owner = other->GetOwner();
    if (owner && owner->GetObjectType() == ObjectType::Player) {
        isCollected = true;

        auto cardManager = Globals::GetGlobals().cardManager;

        if (cardManager) {
            std::shared_ptr<Card> newCard = cardManager->CreateCard(cardTypeToUnlock);
            newCard->AssignPlayer(player);
            cardManager->LearnCard(newCard);
        }
        std::string id = this->GetSaveID();
        if (!id.empty()) {
            auto& globals = Globals::GetGlobals();
            if (globals.worldStateManager) {
                std::string currentLevel = globals.activeLevelName;
                globals.worldStateManager->MarkAsDestroyed(currentLevel, id);
            }
        }

		this->Disable();
    }
}


void CardFind::Init(shared_ptr<Scene> scene) {
    player = scene->GetPlayer();
}