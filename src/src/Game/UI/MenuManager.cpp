#include "include/Game/UI/MenuManager.hpp"

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
	// TBD
}

bool MenuManager::Input(InputEvent& event)
{
	return false;
}