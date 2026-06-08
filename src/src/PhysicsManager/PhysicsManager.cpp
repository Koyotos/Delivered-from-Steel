#include "include/PhysicsManager/PhysicsManager.hpp"
#include "include/Core/Scene.hpp"
#include <algorithm>
#include <unordered_set>

void PhysicsManager::Update(shared_ptr<Scene> scene, float dt) {
    if (currentScene != scene) {
        currentScene = scene;
        currentNodes.clear();
        UpdateNode(scene->root);
        RecalculateWorldBounds();
    }

    // Update physics
    for (auto& node : currentNodes) {
        if (!node->TestPhysics()) continue;
        node->Physics(dt);
        if (!node->GetStatic()) {
            node->ResetGlobal();
            node->SetTransformChanged(true);
        }
    }

    // Process collisions
    for (auto& node : currentNodes) {
        if (!node->TestPhysics()) continue;
        auto col = node->GetCollider();
        if (col)
            node->processCollisions();
    }

    // Recompute global transforms
    Transform t;
    scene->UpdateTransforms(static_pointer_cast<PhysicsNode>(scene->root), t);

    // Update collider positions
    for (auto& node : currentNodes) {
        if (!node->TestPhysics()) continue;
        auto col = node->GetCollider();
        if (col)
            col->UpdatePosition(node->GetTransform());
    }

    // Build quadtree
    quadTree = QuadTree(0, WorldBounds);
    for (auto& node : currentNodes) {
        if (!node->TestPhysics()) continue;
        auto col = node->GetCollider();
        if (col)
            quadTree.Insert(node.get());
    }

    // Query + resolve collisions
    uint32_t environmentMask = static_cast<uint32_t>(ObjectType::Wall) |
        static_cast<uint32_t>(ObjectType::Trap) |
        static_cast<uint32_t>(ObjectType::BreakableWall);

    vector<PhysicsNode*> candidates;
    vector<pair<PhysicsNode*, CollisionInfo>> actualHits;

    for (auto& node : currentNodes) {
        if (!node->TestPhysics()) continue;
        auto col = node->GetCollider();
        if (!col) continue;

        uint32_t typeA = static_cast<uint32_t>(node->GetObjectType());
        if ((typeA & environmentMask) != 0)
            continue;

        candidates.clear();
        actualHits.clear();
        quadTree.Query(col->GetBounds(), candidates);

        for (auto& other : candidates) {
            if (node.get() == other) continue;

            auto otherCol = other->GetCollider();
            if (!otherCol) continue;

            shared_ptr<CollisionInfo> info = node->GetCollisionInfo(otherCol);

            if (info && info->collided) {
                actualHits.push_back({ other, *info });
            }
        }

        std::sort(actualHits.begin(), actualHits.end(), [](const pair<PhysicsNode*, CollisionInfo>& a, const pair<PhysicsNode*, CollisionInfo>& b) {
            bool aIsDown = a.second.normal.y < 0.0f;
            bool bIsDown = (b.second.normal.y < 0.0f);

            if (aIsDown != bIsDown) {
                return aIsDown;
            }
            return a.second.depth > b.second.depth;
        });

        for (auto& hit : actualHits) {
            node->ResolveCollision(*(hit.first));
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

void PhysicsManager::CollectPhysicsNodes(shared_ptr<Node> node, vector<shared_ptr<PhysicsNode>>& outNodes) {
	if (!node) return;

	auto physicsNode = dynamic_pointer_cast<PhysicsNode>(node);
	if (physicsNode) {
		outNodes.push_back(physicsNode);
	}

	for (const auto& child : node->GetChildren()) {
		CollectPhysicsNodes(child, outNodes);
	}
}

void PhysicsManager::RecalculateWorldBounds() {
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

PhysicsManager& PhysicsManager::GetPhysicsManager() {
	static PhysicsManager instance;
	return instance;
}

void PhysicsManager::Reset() {
    currentScene = nullptr;
    currentNodes.clear();
}

void PhysicsManager::RegisterNode(shared_ptr<Node> node) {
	if (!node) return;

	UpdateNode(node);
	RecalculateWorldBounds();
}

void PhysicsManager::UnregisterNode(shared_ptr<Node> node) {
	if (!node) return;

	vector<shared_ptr<PhysicsNode>> nodesToRemove;
	CollectPhysicsNodes(node, nodesToRemove);

	std::unordered_set<shared_ptr<PhysicsNode>> nodesToRemoveSet(nodesToRemove.begin(), nodesToRemove.end());

    for (auto& removeNode : nodesToRemove) {
        auto colToRemove = removeNode->GetCollider();
        if (!colToRemove) continue;

        for (auto& other : colToRemove->GetCurrentCollisions()) {
            if (other) other->GetCurrentCollisions().erase(colToRemove);
        }

        for (auto& other : colToRemove->GetPreviousCollisions()) {
            if (other) other->GetPreviousCollisions().erase(colToRemove);
        }
        colToRemove->GetCurrentCollisions().clear();
        colToRemove->GetPreviousCollisions().clear();
    }

    currentNodes.erase(std::remove_if(currentNodes.begin(), currentNodes.end(),
        [&nodesToRemoveSet](const shared_ptr<PhysicsNode>& node) {
            return nodesToRemoveSet.find(node) != nodesToRemoveSet.end();
		}), currentNodes.end());

	RecalculateWorldBounds();
}

optional<RaycastHit> PhysicsManager::Raycast(const vec2& origin, const vec2& direction, float maxDistance,
	shared_ptr<Collider> collider, uint32_t type) {

	float closest = maxDistance;
	optional<RaycastHit> result;
	for (size_t i = 0; i < currentNodes.size(); ++i) {
        if (!currentNodes[i]->TestPhysics()) continue;
        if ((static_cast<uint32_t>(currentNodes[i]->GetObjectType()) & type) == 0 && type != static_cast<uint32_t>(ObjectType::Null)) continue;
		auto col = currentNodes[i]->GetCollider();
		if (col && col != collider) {
			auto hit = col->Raycast(origin, direction, maxDistance);
			if (hit && hit->distance < closest) {
				closest = hit->distance;
				result = hit;
			}			
		}
	}
	return result;
}

vector<RaycastHit> PhysicsManager::RaycastAll(const vec2& origin, const vec2& direction, float maxDistance,
	shared_ptr<Collider> collider, uint32_t type) {

	float closest = maxDistance;
	vector<RaycastHit> result;
	for (size_t i = 0; i < currentNodes.size(); ++i) {
        if (!currentNodes[i]->TestPhysics()) continue;
		if ((static_cast<uint32_t>(currentNodes[i]->GetObjectType()) & type) == 0 && type != static_cast<uint32_t>(ObjectType::Null)) continue;
		auto col = currentNodes[i]->GetCollider();
		if (col && col != collider) {
			auto hit = col->Raycast(origin, direction, maxDistance);
			if (hit && hit->distance < closest) {
				closest = hit->distance;
				result.push_back(*hit);
			}
		}
	}
	return result;
}