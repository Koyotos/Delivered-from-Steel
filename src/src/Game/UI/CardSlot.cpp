#include "include/Game/UI/CardSlot.hpp"

void CardSlot::RemoveCard() {

	// play animation of card leaving slot TBD

	card = nullptr;
}

void CardSlot::SetCard(std::shared_ptr<Card> newCard) {
	card = newCard;
	card->SetTransform(this->GetTransform());
}

void CardSlot::Draw() {

	Object2D::Draw();
	if (card) {
		card->Draw();
	}
	if (icon) {
		icon->Draw();
	}
}

CardSlot::CardSlot(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {}
