#ifndef FE_PHYSICS_MANAGER
#define FE_PHYSICS_MANAGER

#include "include/Core/Scene.hpp"
#include "include/PhysicsManager/PhysicsNode.hpp"


class PhysicsManager {

private:
	shared_ptr<Scene> currentScene;
	vector<shared_ptr<PhysicsNode>> currentNodes;

	void updateNode(shared_ptr<Node> node);


public:
	void Update(shared_ptr<Scene> scene, float dt);

	static PhysicsManager& GetPhysicsManager();

	std::optional<RaycastHit> raycast(
		const glm::vec2& origin,
		const glm::vec2& direction,
		float maxDistance,
		std::shared_ptr<Collider> collider,
		ObjectType type);

	std::vector<RaycastHit> raycastAll(
		const glm::vec2& origin,
		const glm::vec2& direction,
		float maxDistance,
		std::shared_ptr<Collider> collider,
		ObjectType type);

	PhysicsManager() = default;
	PhysicsManager(const PhysicsManager&) = delete;
};

#endif