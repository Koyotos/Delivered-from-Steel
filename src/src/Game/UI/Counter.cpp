#include "include/Game/UI/Counter.hpp"

Counter::Counter(shared_ptr<TextUI> text, shared_ptr<Icon> icon, int maxVal)
    : text(text), icon(icon), maxVal(maxVal), currentVal(0) {}

Counter::~Counter() {}

void Counter::UpdateValue(int newVal) {
    currentVal = newVal;
    if (newVal < 10) {
        text->SetTextPos(ivec2(origTextPosX + 9.0f, text->GetTransform().GetTranslation().y)); // Adjust position for single-digit numbers
    } else {
        text->SetTextPos(ivec2(origTextPosX, text->GetTransform().GetTranslation().y)); // Reset to original position for double-digit numbers
	}
    text->SetContent(std::to_string(currentVal));
    // Add animation TBD
    // update icon
}

void Counter::SetText(shared_ptr<TextUI> newText) {
    text = newText;
	origTextPosX = text->GetLeftBound();
}

void Counter::SetIcon(shared_ptr<Icon> newIcon) {
    icon = newIcon;
}

void Counter::SetManaIcon(shared_ptr<Icon> newIcon) {
    manaIcon = newIcon;
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
