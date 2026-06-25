#ifndef FE_PAUSE_MANAGER
#define FE_PAUSE_MANAGER

#include "include/Core/Node.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Globals/Globals.hpp"

class PauseManager : public Node
{
	private:

	int selected;

	bool isPaused;
	bool transitionToRestart = false;
	bool transitionToMenu = false;
	bool axisHeldY = false;

	void FindNodes(shared_ptr<Node>);

	shared_ptr<Scene> pauseScene;

	shared_ptr<Icon> ButtonsBackground;
	shared_ptr<Icon> background;
	vector<shared_ptr<Icon>> buttonIcon;
	vector<shared_ptr<Icon>> buttonText;
	vec3 baseButtonColor;

	shared_ptr<Transition> transition;
	function<void()> onRestart;
	function<void()> onQuit;

	void UpdateButtons();

	public:

	PauseManager();
	~PauseManager();
	void Init(shared_ptr<ResourceManager> rsm);
	void PauseGame();
	bool Input(InputEvent& event) override;
	void Process() override;
	shared_ptr<Scene> GetScene();
	void SetOnRestart(function<void()> cb);
	void SetOnQuit(function<void()> cb);

};

#endif
