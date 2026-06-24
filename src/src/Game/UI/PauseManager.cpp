#include "include/Game/UI/PauseManager.hpp"

#include "include/Renderer/Renderer.hpp"

PauseManager::PauseManager()
{
	SetProcess(true);
	SetInput(true);

	isPaused = false;
}

PauseManager::~PauseManager()
{
	
}

void PauseManager::Init(shared_ptr<ResourceManager> rsm)
{
	pauseScene = rsm->LoadScene("res/scenes/pause.json");
	FindNodes(pauseScene->GetRoot());

}

void PauseManager::FindNodes(shared_ptr<Node> node)
{
	if (!node) return;


	for (auto& k : node->GetChildren()) {
		FindNodes(k);
	}
}

shared_ptr<Scene> PauseManager::GetScene()
{
	return pauseScene;
}

bool PauseManager::Input(InputEvent& event)
{
	if (!event.handled)
	{
		if (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS)
		{
			PauseGame();
			event.handled = true;
			return true;
		}
		if (event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_START && event.action == GLFW_PRESS)
		{
			PauseGame();
			event.handled = true;

			return true;
		}

	}
	return false;
}

void PauseManager::PauseGame()
{
	if (!isPaused)
	{
		Globals::GetGlobals().isPaused = true;
		Globals::GetGlobals().lockPlayerMovement = true;
		// show menu
		isPaused = true;
	}
	else
	{
		Globals::GetGlobals().isPaused = false;
		Globals::GetGlobals().lockPlayerMovement = false;

		isPaused = false;
	}
}
