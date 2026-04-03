#include "include/IOManager/IOManager.hpp"
#include "include/Globals/Globals.hpp"

void IOManager::Init(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
}

void IOManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	IOManager* manager = static_cast<IOManager*>(glfwGetWindowUserPointer(window));
	if (manager) {
		InputEvent evt;
		evt.type = InputType::KEYBOARD;
		evt.key = key;
		evt.action = action;
		manager->ProcessEvent(evt);
	}
}

void IOManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	IOManager* manager = static_cast<IOManager*>(glfwGetWindowUserPointer(window));
	if (manager) {
		InputEvent evt;
		evt.type = InputType::MOUSE_BUTTON;
		evt.key = button;
		evt.action = action;
		manager->ProcessEvent(evt);
	}
}

void IOManager::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	IOManager* manager = static_cast<IOManager*>(glfwGetWindowUserPointer(window));
	if (manager) {
		InputEvent evt;
		evt.type = InputType::MOUSE_MOVE;
		evt.valueX = static_cast<float>(xpos);
		evt.valueY = static_cast<float>(ypos);
		manager->ProcessEvent(evt);
	}
}

void IOManager::PollGamepad() {
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		if (!isGamepadConnected) {
			isGamepadConnected = true;
			Globals::GetGlobals().Log("Gamepad connected.");
		}

		for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_DPAD_RIGHT; i++) {
			if (state.buttons[i] != lastGamepadState.buttons[i]) {
				InputEvent evt;
				evt.type = InputType::GAMEPAD_BUTTON;
				evt.key = i;
				evt.action = state.buttons[i] ? GLFW_PRESS : GLFW_RELEASE;
				ProcessEvent(evt);
			}
		}

		for (int i = 0; i <= GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER; i++) {
			if (state.axes[i] != lastGamepadState.axes[i]) {
				InputEvent evt;
				evt.type = InputType::GAMEPAD_AXIS;
				evt.key = i;
				evt.valueX = state.axes[i];
				ProcessEvent(evt);
			}
		}
		lastGamepadState = state;
	}
	else {
		isGamepadConnected = false;
	}
}

void IOManager::ProcessEvent(const InputEvent& event) {
	eventQueue.push_back(event);

	if (event.type == InputType::KEYBOARD) {
		if (event.action == GLFW_PRESS) Globals::GetGlobals().SetKeyState(event.key, true);
		else if (event.action == GLFW_RELEASE) Globals::GetGlobals().SetKeyState(event.key, false);
	}
	else if (event.type == InputType::MOUSE_BUTTON) {
		if (event.action == GLFW_PRESS) Globals::GetGlobals().SetMouseState(event.key, true);
		else if (event.action == GLFW_RELEASE) Globals::GetGlobals().SetMouseState(event.key, false);
	}
	else if (event.type == InputType::MOUSE_MOVE) {
		Globals::GetGlobals().SetMousePos(event.valueX, event.valueY);
	}
	else if (event.type == InputType::GAMEPAD_BUTTON) {
		if (event.action == GLFW_PRESS) Globals::GetGlobals().SetGamepadBtnState(event.key, true);
		else if (event.action == GLFW_RELEASE) Globals::GetGlobals().SetGamepadBtnState(event.key, false);
	}
	else if (event.type == InputType::GAMEPAD_AXIS) {
		Globals::GetGlobals().SetGamepadAxisState(event.key, event.valueX);
	}
}

void IOManager::ProcessInput(shared_ptr<Node> node) {
	if (!node) return;

	if (node->TestInput()) {
		for (auto& event : eventQueue) {
			if (node->Input(event)) {
				event.handled = true;
			}
		}
	}

	for (auto& child : node->GetChildren()) {
		ProcessInput(child);
	}
}

void IOManager::ClearQueue() {
	eventQueue.clear();
}
