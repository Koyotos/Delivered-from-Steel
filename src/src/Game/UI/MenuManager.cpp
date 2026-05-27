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
 		logo->MoveTo(vec2(logo->GetTransform().GetGlobal()[3].x, logo->GetTransform().GetGlobal()[3].y - 105.0f), 2.0f, EaseType::OutQuad, 0.25f);
		logo->FadeIn(2.0f, EaseType::Linear, 0.25f);
	}

	init = true;


}

MenuManager::MenuManager()
{
	SetProcess(true);
	init = false;
}

MenuManager::~MenuManager()
{
}

void MenuManager::Process()
{
	//TBD
}

bool MenuManager::Input(InputEvent& event)
{
	if (!event.handled) {
		if ((event.type == InputType::KEYBOARD || event.type == InputType::GAMEPAD_BUTTON) && event.action == GLFW_PRESS) {
			event.handled = true;
			if (init) {
				if (logo->GetActiveTweens().empty())
				{
					ToMainMenu();
				}
				else
				{
					logo->FinishAllTweens();
				}
			} else if (toMainMenu && (!logo->GetActiveTweens().empty() || !platform->GetActiveTweens().empty() || !buttonIcons[0]->GetActiveTweens().empty() /* || !buttonText->GetActiveTweens().empty()*/)) {

				logo->FinishAllTweens();
				platform->FinishAllTweens();
				for (auto& icon : buttonIcons) {
					icon->FinishAllTweens();
				}
				// buttonText->FinishAllTweens();

			} else if (toMainMenu) {
				switch (selectedButton)
				{ 
					case 0: if (onStartGame) onStartGame(); break;
					default: break;
				}
				
			}
			return true;
		}
	}
	return false;
}

void MenuManager::FindNodes(shared_ptr<Node> node) {
	if (!node) return;

	if (auto cast = dynamic_pointer_cast<Icon>(node)) {
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
	}
	if (auto cast = dynamic_pointer_cast<TextUI>(node)) {
		if (cast->GetName() == "buttonText") {
			buttonText.push_back(cast);
			std::sort(buttonText.begin(), buttonText.end(), [](const std::shared_ptr<TextUI>& a, const std::shared_ptr<TextUI>& b) {return a->GetTransform().GetGlobal()[3].y < b->GetTransform().GetGlobal()[3].y; });
		}
	}

	for (auto& k : node->GetChildren()) {
		FindNodes(k);
	}
}

void MenuManager::ToMainMenu()
{
	if (logo) logo->MoveTo(vec2(logo->GetTransform().GetGlobal()[3].x, 10.0f), 2.0f, EaseType::OutQuad, 0.25f);
	if (platform)
	{
		platform->MoveTo(vec2(platform->GetTransform().GetGlobal()[3].x, 600.0f), 4.0f, EaseType::Linear);
		// change color
	}
	if (!buttonIcons.empty())
	{
		// update + fade in
	}
	if (!buttonText.empty())
	{
		// update
		for (auto& text : buttonText) {
			text->FadeIn(0.5f, EaseType::OutSine, 1.5f);
		}
	}
	toMainMenu = true;
}
