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

void UIElement::SetTint(glm::vec3 color) {
	tint = color;
}

glm::vec3 UIElement::GetTint() const {
	return tint;
}

void UIElement::UpdateTransform() {
	return;
}



UIElement::UIElement(const std::unordered_map<std::string, std::any>& data) : Object2D(data) {
	isVisible = fromMap(bool, "visible", data);
	alpha = fromMap(float, "alpha", data);

	tint = glm::vec3(1.0f, 1.0f, 1.0f);

	SetDraw(true);

}

UIElement::UIElement() : Object2D() {
	tint = glm::vec3(1.0f, 1.0f, 1.0f);
	SetDraw(true);
}