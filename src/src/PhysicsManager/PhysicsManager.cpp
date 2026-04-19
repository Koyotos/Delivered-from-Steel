#include "include/PhysicsManager/PhysicsManager.hpp"


void PhysicsManager::Update(shared_ptr<Scene> scene, float dt) {

	// jesli scena sie zmienila to pobierz nowe PhysicsNode'y
	if (currentScene != scene) {
		currentScene = scene;
		currentNodes.clear();
		updateNode(scene->root);
	}

	// ruch
	for (const auto& physicsNode : currentNodes) {
		physicsNode->Update(dt);
		if (!physicsNode->getStatic()) {
			physicsNode->ResetGlobal();
			physicsNode->SetTransformChanged(true);
		}
	}

	// kolizje
	for (size_t i = 0; i < currentNodes.size(); ++i) {
		auto col = currentNodes[i]->GetCollider();
		if (col) {
			col->updatePosition(currentNodes[i]->GetTransform());
		}


		for (size_t j = 0; j < currentNodes.size(); ++j) {
			if (i == j) continue;
			currentNodes[i]->resolveCollision(*currentNodes[j]);
		}

		if (col) {
			currentNodes[i]->processCollisions();
		}
	}

	return;
}

void PhysicsManager::updateNode(std::shared_ptr<Node> node) {
	auto physicsNode = dynamic_pointer_cast<PhysicsNode>(node);
	if (physicsNode) {
		currentNodes.push_back(physicsNode);
		physicsNode->Init();
	}

	for (const auto& child : node->GetChildren()) {
		updateNode(child);
	}	
}

PhysicsManager& PhysicsManager::GetPhysicsManager()
{
	static PhysicsManager instance;
	return instance;
}

std::optional<RaycastHit> PhysicsManager::raycast(
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
			auto hit = col->raycast(origin, direction, maxDistance);

			if (hit && hit->distance < closest)
			{
				closest = hit->distance;
				result = hit;
			}			
		}
	}

	return result;
}

std::vector<RaycastHit> PhysicsManager::raycastAll(
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
			auto hit = col->raycast(origin, direction, maxDistance);

			if (hit && hit->distance < closest)
			{
				closest = hit->distance;
				result.push_back(*hit);
			}
		}
	}

	return result;
}