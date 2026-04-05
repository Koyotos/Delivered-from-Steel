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
		if (auto col = currentNodes[i]->GetCollider()) {

			col->setCurrentToPrevious();
			if (!currentNodes[i]->getStatic()) {
				col->updatePosition(currentNodes[i]->GetTransform());
			}
		}


		for (size_t j = i + 1; j < currentNodes.size(); ++j) {
			currentNodes[i]->resolveCollision(*currentNodes[j]);
		}
	}
	return;
}

void PhysicsManager::updateNode(std::shared_ptr<Node> node) {
	auto physicsNode = dynamic_pointer_cast<PhysicsNode>(node);
	if (physicsNode) {
		currentNodes.push_back(physicsNode);
	}

	for (const auto& child : node->GetChildren()) {
		updateNode(child);
	}	
}