#include "include/Game/UI/CardSlot.hpp"
#include "include/Globals/Globals.hpp"
#include "GLFW/glfw3.h"


void CardSlot::SetCard(std::shared_ptr<CardUI> newCard) {
	// if (card) zwrocic, pozniej bedzie handlowana animacja w cardmanagerze

	// pozniej zamienic to na moveTo i RotateTo z animacja, a nie od razu ustawiac nowy transform
	card = newCard;
	card->SetVisible(true);
    card->MoveTo(vec2(this->GetTransform().GetTranslation().x, this->GetTransform().GetTranslation().y), 0.2f, EaseType::InOutSine);    
    card->RotateTo(this->GetTransform().GetRotation().z, 0.2f, EaseType::InOutSine);
}


void CardSlot::Draw(shared_ptr<Shader> sh) {	
	UIElement::Draw();
	if (icon) icon->Draw(sh);
	if (card) card->Draw(GetShader()); 

}

shared_ptr<CardUI> CardSlot::RemoveCard() {
    if (!card) return nullptr;
	shared_ptr<CardUI> removedCard = card;
	card = nullptr;
	return removedCard;
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

void CardSlot::MoveTo(glm::vec2 target, float time, EaseType ease, float delay) {
    UIElement::MoveTo(target, time, ease, delay);
    if (icon) icon->MoveTo(target, time, ease, delay);
    if (card) card->MoveTo(target, time, ease, delay);
}

void CardSlot::FinishAllTweens() {
    UIElement::FinishAllTweens();
    if (icon) icon->FinishAllTweens();
    if (card) card->FinishAllTweens();
}

void CardSlot::ScaleCardTo(glm::vec2 target, float time, EaseType ease, float delay) {
    if (card)
    {
        vec2 origScale = {
			2.7f, 2.7f
        };
        if (origPos == vec2(0.0f, 0.0f)) origPos = {
            card->GetTransform().GetTranslation().x,
            card->GetTransform().GetTranslation().y
        };

        const vec2 halfSize = vec2(75.f / 2.f, 100.f / 2.f);

        auto CompensatedPos = [&](vec2 newScale) -> vec2 {
            return origPos - (newScale - origScale) * halfSize;
            };

        vec2 s1 = { origScale.x * target.x, origScale.y * target.y };

        card->ScaleTo(s1, time, ease, delay);
        card->MoveTo(CompensatedPos(s1), time, ease, delay);
    }
    else
    {

        vec2 origScale = {
            2.7f, 2.7f
        };
        if (origPos == vec2(0.0f, 0.0f)) origPos = {
            GetTransform().GetTranslation().x,
            GetTransform().GetTranslation().y
        };

        const vec2 halfSize = vec2(75.f / 2.f, 100.f / 2.f);

        auto CompensatedPos = [&](vec2 newScale) -> vec2 {
            return origPos - (newScale - origScale) * halfSize;
            };

        vec2 s1 = { origScale.x * target.x, origScale.y * target.y };

        ScaleTo(s1, time, ease, delay);
        MoveTo(CompensatedPos(s1), time, ease, delay);
    }
}
