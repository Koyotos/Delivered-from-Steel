#include "include/IOManager/IOManager.hpp"
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#include <Xinput.h>
#elif defined(__linux__)
#include <linux/input.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#endif

void IOManager::Init(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);

#ifdef __linux__
	for (int i = 0; i < 32; ++i) {
		char path[64];
		snprintf(path, sizeof(path), "/dev/input/event%d", i);
		int fd = open(path, O_RDWR | O_NONBLOCK);
		if (fd >= 0) {
			unsigned long features[4] = { 0 };
			ioctl(fd, EVIOCGBIT(EV_FF, sizeof(features)), features);
			if (features[0] & (1 << FF_RUMBLE)) {
				ff_fd = fd;
				break;
			}
			close(fd);
		}
	}
#endif
}

IOManager::~IOManager() {
#ifdef __linux__
	if (ff_fd >= 0) {
		close(ff_fd);
	}
#endif
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

		for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
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

void IOManager::SetMotorSpeed(float leftMotor, float rightMotor) {
	leftMotor = std::clamp(leftMotor, 0.0f, 1.0f);
	rightMotor = std::clamp(rightMotor, 0.0f, 1.0f);

#ifdef _WIN32
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotor * 65535.0f);
	vibration.wRightMotorSpeed = static_cast<WORD>(rightMotor * 65535.0f);
	XInputSetState(0, &vibration);
#elif defined(__linux__)
	if (ff_fd < 0) return;

	ff_effect effect;
	memset(&effect, 0, sizeof(effect));
	effect.type = FF_RUMBLE;
	effect.id = ff_effect_id;
	effect.u.rumble.strong_magnitude = static_cast<uint16_t>(leftMotor * 65535.0f);
	effect.u.rumble.weak_magnitude = static_cast<uint16_t>(rightMotor * 65535.0f);
	effect.replay.length = 0;
	effect.replay.delay = 0;

	if (ioctl(ff_fd, EVIOCSFF, &effect) != -1) {
		ff_effect_id = effect.id;

		input_event play;
		memset(&play, 0, sizeof(play));
		play.type = EV_FF;
		play.code = ff_effect_id;
		play.value = (leftMotor == 0.0f && rightMotor == 0.0f) ? 0 : 1;

		write(ff_fd, (const void*)&play, sizeof(play));
	}
#endif
}

void IOManager::Vibrate(float leftMotor, float rightMotor, float duration) {
	if (!isGamepadConnected) return;

	if (isVibrating && duration <= vibrationTimer) {
		return;
	}
	SetMotorSpeed(leftMotor, rightMotor);
	vibrationTimer = duration;
	isVibrating = true;
}

void IOManager::UpdateVibration(float deltaTime) {
	if (isVibrating) {
		vibrationTimer -= deltaTime;
		if (vibrationTimer <= 0.0f) {
			SetMotorSpeed(0.0f, 0.0f);
			isVibrating = false;
		}
	}
}

void IOManager::ClearQueue() {
	eventQueue.clear();
}
