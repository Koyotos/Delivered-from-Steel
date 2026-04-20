#include "include/Game/UI/CardSlot.hpp"
#include "include/Globals/Globals.hpp"
#include "GLFW/glfw3.h"

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

void CardSlot::Process() {
	UIElement::Process();


}

bool CardSlot::Input(InputEvent& event) {
	if (!event.handled) {
		bool isUsedKeyU = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_U);
		bool isUsedKeyJ = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_J);

		if(isUsedKeyU) {
			event.handled = true;
			UIElement::MoveTo(glm::vec2(GetTransform().GetGlobal()[3].x, GetTransform().GetGlobal()[3].y - 100.0f), 0.5f, EaseType::InOutQuad);
			UIElement::FadeOut(0.5f);
		}
		else if(isUsedKeyJ) {
			event.handled = true;
			UIElement::MoveTo(glm::vec2(GetTransform().GetGlobal()[3].x, GetTransform().GetGlobal()[3].y + 100.0f), 0.5f, EaseType::OutSine);
			UIElement::FadeIn(0.5f);
		}
	}
	return false;
}
