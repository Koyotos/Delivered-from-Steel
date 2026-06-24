#ifndef FE_PAUSE_MANAGER
#define FE_PAUSE_MANAGER

#include "include/Core/Node.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Globals/Globals.hpp"

class PauseManager : public Node
{
	private:

	bool isPaused;

	void FindNodes(shared_ptr<Node>);

	shared_ptr<Scene> pauseScene;

	public:

	PauseManager();
	~PauseManager();
	void Init(shared_ptr<ResourceManager> rsm);
	void PauseGame();
	bool Input(InputEvent& event) override;
	shared_ptr<Scene> GetScene();

};

#endif
