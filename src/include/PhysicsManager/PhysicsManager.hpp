#ifndef FE_PHYSICS_MANAGER
#define FE_PHYSICS_MANAGER

#include "include/PhysicsManager/PhysicsNode.hpp"
#include "include/PhysicsManager/AABB.hpp"
#include "include/PhysicsManager/QuadTree.hpp"

class PhysicsManager {

	private:
	shared_ptr<Scene> currentScene;
	vector<shared_ptr<PhysicsNode>> currentNodes;
	void UpdateNode(shared_ptr<Node> node);
	void CollectPhysicsNodes(shared_ptr<Node> node, vector<shared_ptr<PhysicsNode>>& outNodes);
	void RemoveColliderReferences(const shared_ptr<Collider>& collider);
	void RecalculateWorldBounds();
	AABB WorldBounds;
	QuadTree quadTree;

	public:
	void Update(shared_ptr<Scene> scene, float dt);

	static PhysicsManager& GetPhysicsManager();

	void Reset();
	void RegisterNode(shared_ptr<Node> node);
	void UnregisterNode(shared_ptr<Node> node);

	optional<RaycastHit> Raycast(
		const vec2& origin,
		const vec2& direction,
		float maxDistance,
		shared_ptr<Collider> collider,
		uint32_t type);

	vector<RaycastHit> RaycastAll(
		const vec2& origin,
		const vec2& direction,
		float maxDistance,
		shared_ptr<Collider> collider,
		uint32_t type);

	PhysicsManager() = default;
	PhysicsManager(const PhysicsManager&) = delete;
};

#endif