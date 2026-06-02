#include "include/Game/UI/Counter.hpp"

Counter::Counter(shared_ptr<TextUI> text, shared_ptr<Icon> icon, int maxVal)
    : text(text), icon(icon), maxVal(maxVal), currentVal(0) {}

Counter::~Counter() {}

void Counter::UpdateValue(int newVal) {
    currentVal = newVal;
    text->SetContent(std::to_string(currentVal));
    // play animation TBD
    // update icon
}

void Counter::SetText(shared_ptr<TextUI> newText) {
    text = newText;
}

void Counter::SetIcon(shared_ptr<Icon> newIcon) {
    icon = newIcon;
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
