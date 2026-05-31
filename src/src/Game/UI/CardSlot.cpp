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

void CardSlot::PlayUseAnimation() {
    if (!card) return;

    card->FinishAllTweens();

    vec2 origScale = {
        card->GetTransform().GetScale().x,
        card->GetTransform().GetScale().y
    };
    vec2 origPos = {
        card->GetTransform().GetTranslation().x,
        card->GetTransform().GetTranslation().y
    };

    const vec2 halfSize = vec2(75.f / 2.f, 100.f / 2.f);

    auto CompensatedPos = [&](vec2 newScale) -> vec2 {
        return origPos - (newScale - origScale) * halfSize;
        };

    vec2 s1 = { origScale.x * 0.88f, origScale.y * 0.95f };
    vec2 s2 = { origScale.x * 1.1f,  origScale.y * 1.05f };

    card->ScaleTo(s1, 0.12f, EaseType::OutSine);
    card->MoveTo(CompensatedPos(s1), 0.12f, EaseType::OutSine);

    card->ScaleTo(s2, 0.14f, EaseType::InOutSine, 0.12f);
    card->MoveTo(CompensatedPos(s2), 0.14f, EaseType::InOutSine, 0.12f);

    card->ScaleTo(origScale, 0.16f, EaseType::OutElastic, 0.26f);
    card->MoveTo(origPos, 0.16f, EaseType::OutElastic, 0.26f);
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

void CardSlot::SetCardTint(vec3 color) {
    if (card) card->Tint(color, 0.2f, EaseType::Linear);
}

