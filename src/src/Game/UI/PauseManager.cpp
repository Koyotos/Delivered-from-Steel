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
		else if (cast->GetName() == "background")
		{
			background = cast;
		}
		else if (cast->GetName() == "text")
		{
			shared_ptr<Icon> cast = static_pointer_cast<Icon>(node);
			buttonText.push_back(cast);
			baseButtonColor = buttonText[0]->GetTint();
			std::sort(buttonText.begin(), buttonText.end(), [](const std::shared_ptr<Icon>& a, const std::shared_ptr<Icon>& b) {return a->GetTransform().GetGlobal()[3].y < b->GetTransform().GetGlobal()[3].y; });
		}
		else if (cast->GetName() == "buttonIcon")
		{
			buttonIcon.push_back(cast);
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

void PauseManager::Process()
{
	if (transitionToRestart)
	{
		if (transition->GetActiveTweens().empty())
		{
			PauseGame();
			onRestart();
			transitionToRestart = false;
		}
	}
	if (transitionToMenu)
	{
		if (transition->GetActiveTweens().empty())
		{
			PauseGame();
			onRestart();
			transitionToMenu = false;
		}
	}
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

		if (isPaused && (event.type == InputType::KEYBOARD || event.type == InputType::GAMEPAD_BUTTON) && event.action == GLFW_PRESS)
		{
			bool up = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_UP) ||
				(event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_DPAD_UP);
			bool down = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_DOWN) ||
				(event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
			bool confirm = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_ENTER) ||
				(event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_A);

			if (up) {
				selected = (selected - 1 + (int)buttonText.size()) % (int)buttonText.size();
				UpdateButtons();
				event.handled = true;
				return true;
			}
			if (down) {
				selected = (selected + 1) % (int)buttonText.size();
				UpdateButtons();
				event.handled = true;
				return true;
			}
			if (confirm) {
				switch (selected) {
				case 0: PauseGame();       break;  // Resume
				case 1: transitionToRestart = true; transition->ChangeState(0.0f);  break;
				case 2: transitionToMenu = true; transition->ChangeState(0.0f);  break;
				}
				event.handled = true;
				return true;
			}
		}
	}
	return false;
}

void PauseManager::UpdateButtons()
{
	for (int i = 0; i < (int)buttonText.size(); i++) {
		if (i == selected) {
			buttonText[i]->Tint(vec3(1.0f, 0.9f, 0.05f), 0.2f, EaseType::InOutSine);

			float iconOffset;
			if (i == 0)
			{
				iconOffset = 40.0f;
			}
			else
			{
				iconOffset = 5.0f;
			}
			
			float left = buttonText[i]->GetTransform().GetTranslation().x - iconOffset;
			float y = buttonText[i]->GetTransform().GetTranslation().y + 15.0f;

			buttonIcon[0]->MoveTo(vec2(left, y + 4.0f), 0.2f, EaseType::InOutSine);
		}
		else {
			buttonText[i]->Tint(vec3(1.0f, 1.0f, 1.0f), 0.2f, EaseType::InOutSine);
		}
	}
}

void PauseManager::PauseGame()
{
	if (!isPaused)
	{
		selected = 0;
		Globals::GetGlobals().isPaused = true;
		Globals::GetGlobals().lockPlayerMovement = true;
		ButtonsBackground->ClearAllTweens();
		ButtonsBackground->MoveTo(vec2(660.0f, 332.0f), 0.5f, EaseType::InOutSine);
		background->FadeIn(0.5f, EaseType::OutSine);
		buttonIcon[0]->FadeIn(0.1f, EaseType::OutSine, 0.4f);
		for (auto& k : buttonText)
		{
			k->FadeIn(0.1f, EaseType::OutSine, 0.4f);
		}
		UpdateButtons();
		isPaused = true;
	}
	else
	{
		Globals::GetGlobals().isPaused = false;
		Globals::GetGlobals().lockPlayerMovement = false;
		ButtonsBackground->ClearAllTweens();
		ButtonsBackground->MoveTo(vec2(660.0, -600.0f), 0.5f, EaseType::InOutSine);
		background->FadeOut(0.5f, EaseType::OutSine);
		buttonIcon[0]->FadeOut(0.1f, EaseType::OutSine);
		for (auto& k : buttonText)
		{
			k->FadeOut(0.1f, EaseType::OutSine);
		}
		isPaused = false;
	}
}

void PauseManager::SetOnQuit(function<void()> cb)
{
	onQuit = cb;
}

void PauseManager::SetOnRestart(function<void()> cb)
{
	onRestart = cb;
}
