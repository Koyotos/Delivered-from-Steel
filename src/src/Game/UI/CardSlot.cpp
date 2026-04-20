#include "include/Game/UI/CardSlot.hpp"

void CardSlot::RemoveCard() {

	// play animation of card leaving slot TBD

	card = nullptr;
}

void CardSlot::SetCard(std::shared_ptr<CardUI> newCard) {
	card = newCard;
	card->SetTransform(this->GetTransform());
}


void CardSlot::Draw() {
	if (!GetVisible()) return;

	shader->SetVec3("tint", GetTint());
	shader->SetFloat("alpha", GetAlpha());

	// make cleaner later
	shader->SetMat4("VP", glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f));

	Object2D::Draw();
	if (card) {
		card->Draw();
	}
	if (icon) {
		icon->Draw();
	}
}


CardSlot::CardSlot(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {
	SetDraw(true);
}
