#include "include/PhysicsManager/PhysicsManager.hpp"

void PhysicsManager::Update(shared_ptr<Scene> scene, float dt)
{
    if (currentScene != scene) {
        currentScene = scene;
        currentNodes.clear();
        UpdateNode(scene->root);
        WorldBounds.min = vec2(FLT_MAX);
        WorldBounds.max = vec2(-FLT_MAX);
        for (auto& node : currentNodes) {
            auto col = node->GetCollider();
            if (!col) continue;
            auto b = col->GetBounds();
            WorldBounds.min = glm::min(WorldBounds.min, b.min);
            WorldBounds.max = glm::max(WorldBounds.max, b.max);
        }
    }

    // Update physics
    for (auto& node : currentNodes) {
        node->Update(dt);
        if (!node->getStatic()) {
            node->ResetGlobal();
            node->SetTransformChanged(true);
        }
    }

    // Process collisions
    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (col)
            node->processCollisions();
    }

    // Recompute global transforms
    Transform t;
    scene->UpdateTransforms(static_pointer_cast<PhysicsNode>(scene->root), t);

    // Update collider positions
    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (col)
            col->UpdatePosition(node->GetTransform());
    }

    // Build quadtree
    quadTree = QuadTree(0, WorldBounds);
    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (col)
            quadTree.Insert(node);
    }

    // Query + resolve collisions
    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (!col)
            continue;
        std::vector<std::shared_ptr<PhysicsNode>> candidates;
        quadTree.Query(col->GetBounds(), candidates);
        for (auto& other : candidates) {
            if (node.get() == other.get())
                continue;
            node->resolveCollision(*other);
        }
    }
}


void PhysicsManager::UpdateNode(std::shared_ptr<Node> node) {
	auto physicsNode = dynamic_pointer_cast<PhysicsNode>(node);
	if (physicsNode) {
		currentNodes.push_back(physicsNode);
		physicsNode->Init();
	}

	for (const auto& child : node->GetChildren()) {
		UpdateNode(child);
	}	
}

PhysicsManager& PhysicsManager::GetPhysicsManager()
{
	static PhysicsManager instance;
	return instance;
}

std::optional<RaycastHit> PhysicsManager::Raycast(
	const glm::vec2& origin,
	const glm::vec2& direction,
	float maxDistance,
	std::shared_ptr<Collider> collider,
	ObjectType type)
{
	float closest = maxDistance;
	std::optional<RaycastHit> result;

	for (size_t i = 0; i < currentNodes.size(); ++i) {
		if (currentNodes[i]->GetObjectType() != type && type != ObjectType::Null) continue;
		auto col = currentNodes[i]->GetCollider();
		if (col && col != collider) {
			auto hit = col->Raycast(origin, direction, maxDistance);

			if (hit && hit->distance < closest)
			{
				closest = hit->distance;
				result = hit;
			}			
		}
	}

	return result;
}

std::vector<RaycastHit> PhysicsManager::RaycastAll(
	const glm::vec2& origin,
	const glm::vec2& direction,
	float maxDistance,
	std::shared_ptr<Collider> collider,
	ObjectType type)
{
	float closest = maxDistance;
	std::vector<RaycastHit> result;

	for (size_t i = 0; i < currentNodes.size(); ++i) {
		if (currentNodes[i]->GetObjectType() != type && type != ObjectType::Null) continue;
		auto col = currentNodes[i]->GetCollider();
		if (col && col != collider) {
			auto hit = col->Raycast(origin, direction, maxDistance);

			if (hit && hit->distance < closest)
			{
				closest = hit->distance;
				result.push_back(*hit);
			}
		}
	}

	return result;
}