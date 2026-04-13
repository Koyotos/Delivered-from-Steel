#include "include/Game/UI/UIElement.hpp"

void UIElement::SetVisible(bool value) {
	isVisible = value;
}

bool UIElement::GetVisible() const {
	return isVisible;
}

void UIElement::SetAlpha(float value) {
	alpha = value;
}

float UIElement::GetAlpha() const {
	return alpha;
}

void UIElement::UpdateTransform() {
	return;
}