#include "include/Game/Objects/Player.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

Player::Player() : Object2D() {
	SetProcess(true);
	SetInput(true);
	SetDraw(true);
}

Player::Player(const unordered_map<string, std::any>& data) : Object2D(data) {

}

void Player::SetCamera(shared_ptr<Camera> cam) {
	camera = cam;
}

void Player::Process() {
	Transform t = GetTransform();
	vec3 pos = t.GetTranslation();
	float speed = 0.025f;

	//float leftXM = Globals::GetGlobals().GetMouseX();
	//float leftYM = Globals::GetGlobals().GetMouseY();

	//pos.x = leftXM;
	//pos.y = leftYM;

	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_W)) pos.y += speed;
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_S)) pos.y -= speed;
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_A)) pos.x -= speed;
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_D)) pos.x += speed;

	float leftX = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_LEFT_X);
	float leftY = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_LEFT_Y);

	if (std::abs(leftX) > 0.1f) pos.x += leftX * speed;
	if (std::abs(leftY) > 0.1f) pos.y += leftY * speed;

	if (Globals::GetGlobals().GetMouseState(GLFW_MOUSE_BUTTON_LEFT)) pos.x -= speed;
	if (Globals::GetGlobals().GetMouseState(GLFW_MOUSE_BUTTON_RIGHT)) pos.x += speed;

	if (Globals::GetGlobals().GetGamepadBtnState(GLFW_GAMEPAD_BUTTON_A)) pos.y -= speed;

	t.SetTranslation(pos);
	SetTransform(t);
	camera->SetPos(pos);
}

bool Player::Input(InputEvent& event) {
	if (!event.handled) {
		if (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_SPACE && event.action == GLFW_PRESS) {
			Globals::GetGlobals().Log("Skok");
			return true;
		}
	}
	return false;
}
