#include "include/Game/UI/Vignette.hpp"
#include "include/Renderer/Sprite.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>



Vignette::Vignette(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {
	states = fromMap(float, "states", data);

	currentValue = 0;
	minValue = 0;
	maxValue = states;
}

void Vignette::Draw() {
	SetCurrentSprite();

	UIElement::Draw();
}

void Vignette::SetCurrentSprite() {
	GetSprite()->SetActiveTexture(static_cast<int>((currentValue - minValue) / (maxValue - minValue) * states));
}


void Vignette::SetCurrentValue(float value) {
	currentValue = value;
}

void Vignette::SetMinValue(float value) {
	minValue = value;
}

void Vignette::SetMaxValue(float value) {
	maxValue = value;
}

float Vignette::GetCurrentValue() {
	return currentValue;
}

float Vignette::GetMinValue() {
	return minValue;
}

float Vignette::GetMaxValue() {
	return maxValue;
}
