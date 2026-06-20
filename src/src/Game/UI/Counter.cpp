#include "include/Game/UI/Counter.hpp"

Counter::Counter(shared_ptr<TextUI> text, shared_ptr<Icon> icon, int maxVal)
    : text(text), icon(icon), maxVal(maxVal), currentVal(0) {}

Counter::~Counter() {}

void Counter::UpdateValue(int newVal) {
    currentVal = newVal;
    if (currentVal == maxVal)
    {
        this->UpdateManaIcons();
    }
	valueChanged = true;
    if (newVal < 10) {
        text->SetTextPos(ivec2(origTextPosX + 9.0f, text->GetTransform().GetTranslation().y)); // Adjust position for single-digit numbers
    } else {
        text->SetTextPos(ivec2(origTextPosX, text->GetTransform().GetTranslation().y)); // Reset to original position for double-digit numbers
	}
    text->SetContent(std::to_string(currentVal));
    this->UpdateManaIconsValue();

}

void Counter::UpdateMaxVal(int newVal)
{
	maxVal = newVal;
    this->UpdateManaIcons();
}

void Counter::SetText(shared_ptr<TextUI> newText) {
    text = newText;
	origTextPosX = text->GetLeftBound();
}

void Counter::SetIcon(shared_ptr<Icon> newIcon) {
    icon = newIcon;
}

void Counter::AddManaIcon(shared_ptr<Icon> newIcon) {
    manaIcons.push_back(newIcon);
}

void Counter::FinishAllTweens() {
    text->FinishAllTweens();
    icon->FinishAllTweens();
}

void Counter::FadeOut(float time, EaseType ease, float delay) {
    text->FadeOut(time, ease, delay);
    icon->FadeOut(time, ease, delay);
}

void Counter::FadeIn(float time, EaseType ease, float delay) {
    text->FadeIn(time, ease, delay);
    icon->FadeIn(time, ease, delay);
}

void Counter::UpdateManaIcons() {

    float beginX = 625.0f;
    float endX = 1250.0f;
	float iconY = 1005.0f;

    for (int i = 0; i < manaIcons.size(); i++) {
        if (i < maxVal) {
            manaIcons[i]->SetVisible(true);
            float t = (float)i / (float)(maxVal - 1);
            float x = beginX + t * (endX - beginX);
            manaIcons[i]->MoveTo(vec2(x, iconY), 0.01f);
            manaIcons[i]->FadeIn(0.01f);
            manaIcons[i]->Play("unspent", 0.25f);
        } else {
            manaIcons[i]->SetVisible(false);
        }
	}

}

void Counter::UpdateManaIconsValue()
{

	for (int i = 0; i < maxVal; i++) {
		if (i >= currentVal)
		{
			manaIcons[i]->Play("spent", 0.25f, false);
			manaIcons[i]->FadeOut(0.01f, EaseType::Linear, 0.5f);
		}
		else if (i < currentVal)
		{
			manaIcons[i]->Play("unspent", 0.25f);
			manaIcons[i]->FadeIn(0.01f);
		}
	}
    
}
