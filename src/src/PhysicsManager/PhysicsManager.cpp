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
	}

	// kolizje
	for (size_t i = 0; i < currentNodes.size(); ++i) {
		auto col = currentNodes[i]->GetCollider();
		if (col) {
			if (!currentNodes[i]->getStatic()) {
				col->updatePosition(currentNodes[i]->GetTransform());
			}
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