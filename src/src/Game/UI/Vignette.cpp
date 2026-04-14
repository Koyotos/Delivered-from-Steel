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

	// make better later
	shader->SetMat4("VP", glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f));
	Object2D::Draw();
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

