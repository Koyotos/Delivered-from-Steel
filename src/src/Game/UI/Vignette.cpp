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

void Vignette::Draw(shared_ptr<Shader> sh) {
	SetCurrentSprite();

	UIElement::Draw();
}

void Vignette::SetCurrentSprite() {
	GLuint index = static_cast<GLuint>((currentValue - minValue) / (maxValue - minValue) * states);
	this->currentTextureID = index;
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
