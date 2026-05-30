#include "include/Game/UI/CardSlot.hpp"
#include "include/Globals/Globals.hpp"
#include "GLFW/glfw3.h"


void CardSlot::SetCard(std::shared_ptr<CardUI> newCard) {
	// if (card) zwrocic, pozniej bedzie handlowana animacja w cardmanagerze

	// pozniej zamienic to na moveTo i RotateTo z animacja, a nie od razu ustawiac nowy transform
	card = newCard;
	card->SetVisible(true);
	Transform t = this->GetTransform();
	card->SetTransform(t);

}


void CardSlot::Draw(shared_ptr<Shader> sh) {	
	UIElement::Draw();
	if (icon) icon->Draw(sh);
	if (card) card->Draw(GetShader()); 

}

void CardSlot::RemoveCard() {
	if (!card) return;
	card = nullptr;
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

