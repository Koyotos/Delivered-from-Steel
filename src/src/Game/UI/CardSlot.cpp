#include "include/Game/UI/CardSlot.hpp"
#include "include/Globals/Globals.hpp"
#include <iostream>
#include "GLFW/glfw3.h"

void CardSlot::RemoveCard() {
	if (card == nullptr) return;
	card->MoveTo(glm::vec2(GetTransform().GetGlobal()[3].x, GetTransform().GetGlobal()[3].y - 100.0f), 0.5f, EaseType::OutQuad);
	card->FadeOut(0.5f, EaseType::OutQuad);

	removedCard = card;
	card = nullptr;
}

void CardSlot::SetCard(std::shared_ptr<CardUI> newCard) {
	if (card) RemoveCard();

	card = newCard;
	card->SetVisible(true);
	Transform t = this->GetTransform();
	card->SetTransform(t);
	card->SetAlpha(0.0f);
	card->FadeIn(0.5f, EaseType::InQuad);
}


void CardSlot::Draw(shared_ptr<Shader> sh) {	
	UIElement::Draw();
	if (icon) icon->Draw(sh);
	if (card) card->Draw(GetShader()); 
	if (removedCard)
	{
		removedCard->Draw(GetShader());
		if (removedCard->GetAlpha() <= 0.01f) {
			removedCard = nullptr;
		}
	}
}	


CardSlot::CardSlot(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {
	SetDraw(true);
}

string CardSlot::Type()
{
	return "CardSlot";
}

void CardSlot::Process() {
	UIElement::Process();
	if (icon) icon->Process();
	if (card) card->Process();
	if (removedCard) removedCard->Process();
}

