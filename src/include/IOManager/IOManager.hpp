#ifndef FE_IO_MANAGER
#define FE_IO_MANAGER

#include "include/IOManager/InputEvent.hpp"
#include "include/Core/Node.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Renderer/Renderer.hpp"
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

class IOManager {
private:
	std::vector<InputEvent> eventQueue;

	GLFWgamepadstate lastGamepadState;
	bool isGamepadConnected = false;

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

public:
	void Init(GLFWwindow* window);
	void ProcessEvent(const InputEvent& event);
	void ProcessInput(shared_ptr<Node> root, Renderer* rnd);

	void PollGamepad();

	void ClearQueue();
};

#endif