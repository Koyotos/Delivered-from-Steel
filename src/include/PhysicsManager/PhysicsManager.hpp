#ifndef FE_PHYSICS_MANAGER
#define FE_PHYSICS_MANAGER

#include "include/Core/Scene.hpp"
#include "include/PhysicsManager/PhysicsNode.hpp"


class PhysicsManager {

private:
	shared_ptr<Scene> currentScene;
	vector<shared_ptr<PhysicsNode>> currentNodes;

	void updateCurrentNodes();

public:
	void Update(shared_ptr<Scene> scene, float dt);
};

#endif