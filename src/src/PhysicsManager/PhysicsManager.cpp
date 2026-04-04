#include "include/PhysicsManager/PhysicsManager.hpp"

void PhysicsManager::Update(shared_ptr<Scene> scene, float dt) {

	// jesli scena sie zmienila to pobierz nowe PhysicsNode'y
	if (currentScene != scene) {
		currentScene = scene;
		updateCurrentNodes();
	}

	// ruch
	for (const auto& physicsNode : currentNodes) {
		physicsNode->Update(dt);
	}

	// kolizje
	for (size_t i = 0; i < currentNodes.size(); ++i) {
		for (size_t j = i + 1; j < currentNodes.size(); ++j) {
			currentNodes[i]->resolveCollision(*currentNodes[j]);
		}
	}

	return;
}

void PhysicsManager::updateCurrentNodes() {
	currentNodes.clear();
	// pobierz z currentScene wszystkie PhysicsNode i zapisz je w currentNodes
	return;
}