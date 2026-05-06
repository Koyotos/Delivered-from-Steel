#ifndef FE_PHYSICS_MANAGER
#define FE_PHYSICS_MANAGER

#include "include/Core/Scene.hpp"
#include "include/PhysicsManager/PhysicsNode.hpp"

class PhysicsManager {

	private:
	shared_ptr<Scene> currentScene;
	vector<shared_ptr<PhysicsNode>> currentNodes;
	void UpdateNode(shared_ptr<Node> node);

	public:
	void Update(shared_ptr<Scene> scene, float dt);

	static PhysicsManager& GetPhysicsManager();

	optional<RaycastHit> Raycast(
		const vec2& origin,
		const vec2& direction,
		float maxDistance,
		shared_ptr<Collider> collider,
		ObjectType type);

	vector<RaycastHit> RaycastAll(
		const vec2& origin,
		const vec2& direction,
		float maxDistance,
		shared_ptr<Collider> collider,
		ObjectType type);

	PhysicsManager() = default;
	PhysicsManager(const PhysicsManager&) = delete;
};

#endif