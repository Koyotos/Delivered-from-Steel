#include "include/Game/UI/Vignette.hpp"
#include "include/Renderer/Sprite.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>



Vignette::Vignette(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {
	// states = fromMap(float, "states", data); i dont know why this line crashes, investigate later

	states = 3;
	currentValue = 3.0f;
	minValue = 0;
	maxValue = 5;
}

void Vignette::Draw() {
	SetCurrentSprite();

	UIElement::Draw();
}

void Vignette::SetCurrentSprite() {
	GetSprite()->SetActiveTexture(static_cast<int>((currentValue - minValue) / (maxValue - minValue) * states));
}

void Vignette::Process() {

	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_O)) {
		currentValue -= 0.01f;
	}

	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_P)) {
		currentValue += 0.01f;
	}

	if (currentValue < minValue) currentValue = minValue;
	if (currentValue > maxValue) currentValue = maxValue;
}

