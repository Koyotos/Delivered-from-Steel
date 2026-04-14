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

UIElement::UIElement(const std::unordered_map<std::string, std::any>& data) : Object2D(data) {
	isVisible = fromMap(bool, "visible", data);
	alpha = fromMap(float, "alpha", data);
}