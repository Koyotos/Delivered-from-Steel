#include "include/Game/UI/CardSlot.hpp"
#include "include/Globals/Globals.hpp"
#include "GLFW/glfw3.h"


void CardSlot::SetCard(std::shared_ptr<CardUI> newCard) {
	card = newCard;
	card->SetVisible(true);
    card->ScaleTo(vec2(GetTransform().GetScale().x, GetTransform().GetScale().y), 0.2f);
    card->RotateTo(this->GetTransform().GetRotation().z, 0.2f, EaseType::InOutSine);
    card->MoveTo(vec2(this->GetTransform().GetTranslation().x, this->GetTransform().GetTranslation().y), 0.2f, EaseType::InOutSine);
    isLearning = false;
}


void CardSlot::Draw(shared_ptr<Shader> sh) {	
	UIElement::Draw();
	if (isLearning && card) card->Draw(GetShader()); 

}

void CardSlot::Init()
{
	origPos = {
        this->GetTransform().GetTranslation().x,
        this->GetTransform().GetTranslation().y
    };
}

shared_ptr<CardUI> CardSlot::RemoveCard() {
    if (!card) return nullptr;
	shared_ptr<CardUI> removedCard = card;
	card = nullptr;
	return removedCard;
} 

void CardSlot::PlayUseAnimation() {
    if (!card) return;

    card->ClearAllTweens();

    //
    //vec2 origScale = {
    //card->GetTransform().GetScale().x,
    //card->GetTransform().GetScale().y
    //};

    //const vec2 halfSize = vec2(75.f / 2.f, 100.f / 2.f);

    //auto CompensatedPos = [&](vec2 newScale) -> vec2 {
    //    return origPos - (newScale - origScale) * halfSize;
    //    };

    //vec2 s1 = { origScale.x * 0.70f, origScale.y * 0.95f };
    //vec2 s2 = { origScale.x * 1.3f,  origScale.y * 1.05f };

    //card->ScaleTo(s1, 0.12f, EaseType::OutSine);
    //card->MoveTo(CompensatedPos(s1), 0.12f, EaseType::OutSine);

    //card->ScaleTo(s2, 0.14f, EaseType::InOutSine, 0.12f);
    //card->MoveTo(CompensatedPos(s2), 0.14f, EaseType::InOutSine, 0.12f, CompensatedPos(s1));

    //card->ScaleTo(origScale, 0.16f, EaseType::OutElastic, 0.26f);
    //card->MoveTo(origPos, 0.16f, EaseType::OutElastic, 0.26f, CompensatedPos(s2));
	

    
    vec2 targetPos = vec2(origPos.x, origPos.y - 220.0f);
    card->MoveTo(targetPos, 0.3f, EaseType::OutQuad);
    card->MoveTo(vec2(origPos.x, origPos.y), 0.1f, EaseType::OutQuad, 0.35f, targetPos);
    
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
	if (isLearning && card) card->Process();
}

void CardSlot::SetCardTint(vec3 color) {
    if (card) card->Tint(color, 0.2f, EaseType::Linear);
}

void CardSlot::MoveTo(glm::vec2 target, float time, EaseType ease, float delay) {
    UIElement::MoveTo(target, time, ease, delay);
    if (card) card->MoveTo(vec2(target.x, target.y - 41.0f), time, ease, delay);

}

void CardSlot::FinishAllTweens() {
    UIElement::FinishAllTweens();
    if (icon) icon->FinishAllTweens();
    if (card) card->FinishAllTweens();
}

void CardSlot::ClearAllTweens()
{
    UIElement::ClearAllTweens();
    if (icon) icon->ClearAllTweens();
    if (card) card->ClearAllTweens();
}

void CardSlot::ScaleCardTo(glm::vec2 target, float time, EaseType ease, float delay) {

    vec2 origScale = {
    2.7f, 2.7f
    };
    vec2 showPos = {
        origPos.x,
        origPos.y - 250.0f
    };

    const vec2 halfSize = vec2(75.f / 2.f, 100.f / 2.f);

    if (card)
    {
        auto CompensatedPos = [&](vec2 newScale) -> vec2 {
            return vec2(showPos.x, showPos.y-41.0f) - (newScale - origScale) * halfSize;
            };

        vec2 s1 = { origScale.x * target.x, origScale.y * target.y };

        card->ScaleTo(s1, time, ease, delay);
        card->MoveTo(CompensatedPos(s1), time, ease, delay);
    }
    else
    {

        auto CompensatedPos = [&](vec2 newScale) -> vec2 {
            return showPos - (newScale - origScale) * halfSize;
            };

        vec2 s1 = { origScale.x * target.x, origScale.y * target.y };

        ScaleTo(s1, time, ease, delay);
        MoveTo(CompensatedPos(s1), time, ease, delay);
    }
}

void CardSlot::SetIcon(std::shared_ptr<Icon> newIcon) {
    icon = newIcon;
    iconOffset = glm::vec2(icon->GetTransform().GetTranslation().x,
        icon->GetTransform().GetTranslation().y)
        - glm::vec2(GetTransform().GetTranslation().x,
            GetTransform().GetTranslation().y);
}

void CardSlot::ShowSlot(float time)
{
    this->ClearAllTweens();
    this->FadeIn(time, EaseType::InOutSine);
    this->MoveTo(vec2(origPos.x, origPos.y - 250.0f), time, EaseType::InOutSine);
}

void CardSlot::HideSlot(float time)
{
    this->ClearAllTweens();
    this->ScaleCardTo(vec2(1.0f, 1.0f), time);
    this->FadeOut(time, EaseType::InOutSine);
    this->MoveTo(vec2(origPos.x, origPos.y), time, EaseType::InOutSine);
    if (card) card->MoveTo(vec2(origPos.x, origPos.y), time, EaseType::InOutSine);
}

void CardSlot::LearnCard(std::shared_ptr<CardUI> card)
{
  
    if (!card) return;

    // wazne zeby byla ta kolejnosc
	this->SetCard(card);
    isLearning = true;
}