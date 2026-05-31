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
