#include "include/Game/Objects/CardFind.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"

CardFind::CardFind() : Object2D(), isCollected(false) {}

CardFind::CardFind(const unordered_map<string, std::any>& data) : Object2D(data), isCollected(false) {
    int cardType = (int)fromMap(int64_t, "cardType", data);
    cardTypeToUnlock = static_cast<CardType>(cardType);
}

CardFind::~CardFind() {}

std::string CardFind::GetSerializeKey() const {
    glm::vec3 pos = GetTransform().GetTranslation();
    return "cardfind_" + std::to_string((int)pos.x) + "_" + std::to_string((int)pos.y);
}

nlohmann::json CardFind::Serialize() const {
    nlohmann::json j;
    j["isCollected"] = isCollected;
    return j;
}

void CardFind::Deserialize(const nlohmann::json& data) {
    if (data.contains("isCollected")) {
        isCollected = data["isCollected"];

        if (isCollected) {
            this->Disable();
        }
    }
}

void CardFind::OnCollisionEnter(std::shared_ptr<Collider> other) {
    if (isCollected) return;

    std::shared_ptr<PhysicsNode> owner = other->GetOwner();
    if (owner && owner->GetObjectType() == ObjectType::Player) {
        isCollected = true;

        auto cardManager = Globals::GetGlobals().cardManager;

        if (cardManager) {
            std::shared_ptr<Card> newCard = cardManager->CreateCard(cardTypeToUnlock);
            newCard->AssignPlayer(player);
            cardManager->UnlockCard(newCard);
            cardManager->LearnCard(newCard);
        }

		this->Disable();
    }
}


void CardFind::Init(shared_ptr<Scene> scene) {
    player = scene->GetPlayer();
}