#include "include/Game/UI/MenuManager.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>



void MenuManager::SetOnStartGame(function<void()> cb) {
    onStartGame = cb;
}

shared_ptr<Scene> MenuManager::GetMenuScene() {
	return menuScene;
}

void MenuManager::Init(shared_ptr<ResourceManager> rsm)
{

	// load scene
	menuScene = rsm->LoadScene("res/scenes/menu.json");
	FindNodes(menuScene->GetRoot());

	if (logo)
	{
 		logo->MoveTo(vec2(logo->GetTransform().GetGlobal()[3].x, logo->GetTransform().GetGlobal()[3].y - 105.0f), 1.5f, EaseType::OutQuad, 0.25f);
		logo->FadeIn(1.5f, EaseType::Linear, 0.25f);
	}
	if (moon)
	{
		moon->MoveTo(vec2(0.0f, 0.0f), 3.0f, EaseType::OutQuad, 0.25f);
	}

	init = true;


}

MenuManager::MenuManager()
{
	SetProcess(true);
	SetInput(true);
	init = false;
}

MenuManager::~MenuManager()
{
}

void MenuManager::Process()
{
	if (!startPressed) return;
	if (!transition->GetCurrentState())
	{
		if (onStartGame) onStartGame();
	}
}

bool MenuManager::Input(InputEvent& event)
{
	if (!event.handled) {
		if ((event.type == InputType::KEYBOARD || event.type == InputType::GAMEPAD_BUTTON) && event.action == GLFW_PRESS) {
			event.handled = true;
			if (init) {
				if (logo->GetActiveTweens().empty() && moon->GetActiveTweens().empty())
				{
					ToMainMenu();
					init = false;
				}
				else
				{
					logo->FinishAllTweens();
					moon->FinishAllTweens();
				}
			} else if (toMainMenu && (!logo->GetActiveTweens().empty() || !platform->GetActiveTweens().empty() || !buttonIcons[0]->GetActiveTweens().empty()  || !buttonText[0]->GetActiveTweens().empty() || !moon->GetActiveTweens().empty())) {

				logo->FinishAllTweens();
				platform->FinishAllTweens();
				for (auto& icon : buttonIcons) {
					icon->FinishAllTweens();
				}
				for (auto& text : buttonText) {
					text->FinishAllTweens();
				}
				moon->FinishAllTweens();

			} else if (toMainMenu) {
				bool up = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_UP) || (event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_DPAD_UP);
				bool down = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_DOWN) || (event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
				if (up) {
					selectedButton = (selectedButton - 1 + buttonText.size()) % buttonText.size();
					UpdateText();
				}
				else if (down) {
					selectedButton = (selectedButton + 1) % buttonText.size();
					UpdateText();
				}
				else if ((event.type == InputType::KEYBOARD && event.key == GLFW_KEY_ENTER) || (event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_A)) {
					switch (selectedButton) {
					case 0:
						transition->ChangeState(0.0f);
						startPressed = true;
						break;
					case 1:
						// ToOptions();
						break;
					case 2:
						// QuitGame();
						break;
					}
				}
				
			}
			return true;
		}
	}
	return false;
}

void MenuManager::FindNodes(shared_ptr<Node> node) {
	if (!node) return;

	if (node->Type() == "Icon") {
		shared_ptr<Icon> cast = static_pointer_cast<Icon>(node);
		if (cast->GetName() == "logo") {
			logo = cast;
		}
		else if (cast->GetName() == "platform") {
			platform = cast;
			platform->SetTint(vec3(0.1, 0.1, 0.1));
		}
		else if (cast->GetName() == "buttonIcon") {
			buttonIcons.push_back(cast);
		}
		else if (cast->GetName() == "moon") {
			moon = cast;
		}
		else if (cast->GetName() == "buttonsBackground")
		{
			buttonsBackground = cast;
		}
		else if (cast->GetName() == "buttonText")
		{
			shared_ptr<Icon> cast = static_pointer_cast<Icon>(node);
			buttonText.push_back(cast);
			baseButtonColor = buttonText[0]->GetTint();
			std::sort(buttonText.begin(), buttonText.end(), [](const std::shared_ptr<Icon>& a, const std::shared_ptr<Icon>& b) {return a->GetTransform().GetGlobal()[3].y < b->GetTransform().GetGlobal()[3].y; });
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

void MenuManager::ToMainMenu()
{
	if (logo) logo->MoveTo(vec2(logo->GetTransform().GetGlobal()[3].x, 10.0f), 2.0f, EaseType::OutQuad);
	if (platform)
	{
		platform->MoveTo(vec2(platform->GetTransform().GetGlobal()[3].x, 600.0f), 2.5f, EaseType::Linear);
		platform->Tint(vec3(1.0f, 1.0f, 1.0f), 1.0f, EaseType::Linear,  1.5f);
	}
	if (!buttonIcons.empty())
	{
		for (auto& icon : buttonIcons) {
			icon->FadeIn(0.5f, EaseType::OutSine, 2.0f);
		}
	}
	if (buttonsBackground) {
		buttonsBackground->FadeIn(0.5f, EaseType::OutSine, 2.0f);
	}
	if (!buttonText.empty())
	{
		UpdateText();
		for (auto&  text : buttonText) {
			text->FadeIn(0.5f, EaseType::OutSine, 2.0f);
		}
	}
	if (moon) {
		moon->MoveTo(vec2(0.0f, 475.0f), 1.75f, EaseType::InQuad, 0.25f);
	}
	toMainMenu = true;
}

void MenuManager::UpdateText() {
	if (toMainMenu)
		for (int i = 0; i < buttonText.size(); i++) {
			if (i == selectedButton) {
				buttonText[i]->Tint(vec3(1.0f, 0.9f, 0.05f), 0.2f, EaseType::InOutSine);

				float left = buttonText[i]->GetTransform().GetTranslation().x;
				float right = buttonText[i]->GetTransform().GetTranslation().x + buttonText[i]->GetSprite()->GetSize().x;
				float y = buttonText[i]->GetTransform().GetTranslation().y;
				float iconOffset = 56.0f;  

				buttonIcons[0]->MoveTo(vec2(left - iconOffset, y+4.0f), 0.2f, EaseType::InOutSine);
				buttonIcons[1]->MoveTo(vec2(right + 4.0f, y+4.0f), 0.2f, EaseType::InOutSine);
			}
			else {
				buttonText[i]->Tint(baseButtonColor, 0.2f, EaseType::InOutSine);
			}
		}
}