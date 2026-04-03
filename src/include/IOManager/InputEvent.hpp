#ifndef FE_INPUT_EVENT
#define FE_INPUT_EVENT

enum class InputType {
	KEYBOARD,
	MOUSE_BUTTON,
	MOUSE_MOVE,
	GAMEPAD_BUTTON,
	GAMEPAD_AXIS
};

struct InputEvent {
	InputType type;
	int key;
	int action;
	float valueX = 0.0f;
	float valueY = 0.0f;
	bool handled = false;
};

#endif
