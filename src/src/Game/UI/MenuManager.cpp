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
	// menuScene = rsm->LoadScene("res/scenes/menu.json");
	// dodac pozniej logike laczenia tych wszystkich node'ow z menu managerem
	// moze menu scene powinna byc lokalnie??
}

MenuManager::MenuManager()
{
	SetProcess(true);
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
		if (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_SPACE && event.action == GLFW_PRESS) {
			event.handled = true;
			if (onStartGame) {
				onStartGame();
			}
			return true;
		}
	}
	return false;
}