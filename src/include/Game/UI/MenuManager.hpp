#ifndef FE_MENU_MANAGER
#define FE_MENU_MANAGER


#include "include/Core/Node.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Game/UI/Icon.hpp"
#include "include/Game/UI/TextUI.hpp"
#include "include/Game/UI/Transition.hpp"


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
    bool startPressed = false;

    int selectedButton = 0;

	vec3 baseButtonColor;

    float iconRotation = 0.0f;

    shared_ptr<Icon> logo;
    shared_ptr<Icon> platform;
    shared_ptr<Icon> moon;

    shared_ptr<Transition> transition;

    vector<shared_ptr<Icon>> buttonIcons;
	vector<shared_ptr<Icon>> buttonText;

    function<void()> onStartGame;
	shared_ptr<Scene> menuScene;

	void FindNodes(shared_ptr<Node> node);
    void UpdateText();

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

    void ToOptions();
    void QuitGame();


};

#endif
