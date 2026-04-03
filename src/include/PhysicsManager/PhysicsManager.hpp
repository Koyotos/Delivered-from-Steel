#ifndef FE_PHYSICS_MANAGER
#define FE_PHYSICS_MANAGER

#include "include/Core/Scene.hpp"

class PhysicsManager {

public:
	void Update(shared_ptr<Scene> scene, float dt);
};

#endif