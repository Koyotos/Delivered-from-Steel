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

	if (node->Type() == "Icon")
	{
		shared_ptr<Icon> cast = static_pointer_cast<Icon>(node);
		if (cast->GetName() == "buttonsBackground")
		{
			ButtonsBackground = cast;
		}
		if (cast->GetName() == "background")
		{
			background = cast;
		}
	}

	if (node->Type() == "Transition")
	{
		shared_ptr<Transition> cast = static_pointer_cast<Transition>(node);
		transition = cast;
	}


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
		ButtonsBackground->ClearAllTweens();
		ButtonsBackground->MoveTo(vec2(660.0f, 310.0f), 0.5f, EaseType::InOutSine);
		background->FadeIn(0.5f, EaseType::OutSine);
		isPaused = true;
	}
	else
	{
		Globals::GetGlobals().isPaused = false;
		Globals::GetGlobals().lockPlayerMovement = false;
		ButtonsBackground->ClearAllTweens();
		ButtonsBackground->MoveTo(vec2(660.0, -600.0f), 0.5f, EaseType::InOutSine);
		background->FadeOut(0.5f, EaseType::OutSine);
		isPaused = false;
	}
}
