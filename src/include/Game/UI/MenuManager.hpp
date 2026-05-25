#ifndef FE_MENU_MANAGER
#define FE_MENU_MANAGER


#include "include/Core/Node.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Game/UI/Icon.hpp"


enum class MenuButtonEvent
{
    Start,
    Options,
    Quit
};


class MenuManager : public Node
{
private:
    bool init;
    bool toMainMenu;
    bool finishTransition;

    shared_ptr<Icon> logo;
    shared_ptr<Icon> platform;
    vector<shared_ptr<Icon>> buttonIcons;

    function<void()> onStartGame;
	shared_ptr<Scene> menuScene;

public:
    MenuManager();
	~MenuManager();

    void Process() override;
    bool Input(InputEvent& event) override;
    
    void SetOnStartGame(function<void()> cb);

    void Init(shared_ptr<ResourceManager> rsm);
    void ToMainMenu();

    void OnButtonPressed(MenuButtonEvent event);

	shared_ptr<Scene> GetMenuScene();

    void LoadScene();
    void ToOptions();
    void QuitGame();


};

#endif
