#include "include/PhysicsManager/PhysicsManager.hpp"

void PhysicsManager::Update(shared_ptr<Scene> scene, float dt)
{
    // If scene changed, rebuild node list
    if (currentScene != scene) {
        currentScene = scene;
        currentNodes.clear();
        UpdateNode(scene->root);
    }

    //
    // 1. Update physics (movement, velocity, forces)
    //
    for (auto& node : currentNodes) {
        node->Update(dt);
        if (!node->getStatic()) {
            node->ResetGlobal();
            node->SetTransformChanged(true);
        }
    }

    //
    // 2. Recompute global transforms
    //
    Transform t;
    scene->UpdateTransforms(static_pointer_cast<PhysicsNode>(scene->root), t);

    //
    // 3. Update collider bounds (AABB)
    //
    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (col)
            col->UpdatePosition(node->GetTransform());
    }

    //
    // 4. Compute world bounds dynamically
    //
    AABB world;
    world.min = vec2(FLT_MAX);
    world.max = vec2(-FLT_MAX);

    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (!col) continue;

        auto b = col->GetBounds();
        world.min = glm::min(world.min, b.min);
        world.max = glm::max(world.max, b.max);
    }

    //
    // 5. Build quadtree ONCE
    //
    quadTree = QuadTree(0, world);

    //
    // 6. Insert nodes into quadtree
    //
    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (col)
            quadTree.Insert(node);
    }

    //
    // 7. Query + resolve collisions
    //
    for (auto& node : currentNodes) {
        auto col = node->GetCollider();
        if (!col)
            continue;

        std::vector<std::shared_ptr<PhysicsNode>> candidates;
        quadTree.Query(col->GetBounds(), candidates);

        for (auto& other : candidates) {
            if (node.get() >= other.get())
                continue;

            node->resolveCollision(*other);
        }
    }

    /*
	for (size_t i = 0; i < currentNodes.size(); ++i) {
		auto col = currentNodes[i]->GetCollider();
		for (size_t j = i + 1; j < currentNodes.size(); ++j) {
			if (i == j) continue;
			currentNodes[i]->resolveCollision(*currentNodes[j]);
		}
	}
    */
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