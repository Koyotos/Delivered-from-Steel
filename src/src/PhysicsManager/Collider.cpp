#include "include/PhysicsManager/Collider.hpp"
#include "include/PhysicsManager/PhysicsNode.hpp"

Collider::Collider()
    : isTrigger(false), enabled(true), owner(nullptr) {
	currentCollisions = {};
	previousCollisions = {};
}

std::unordered_set<std::shared_ptr<Collider>>& Collider::getCurrentCollisions() {
    return currentCollisions;
}

std::unordered_set<std::shared_ptr<Collider>>& Collider::getPreviousCollisions() {
    return previousCollisions;
}

void Collider::clearCurrentCollisions() {
	currentCollisions.clear();
}

void Collider::addToCurrentCollisions(std::shared_ptr<Collider> col) {
	currentCollisions.insert(col);
}

void Collider::setCurrentToPrevious() {
	previousCollisions = currentCollisions;
	clearCurrentCollisions();
}

glm::vec2 Collider::getGlobalPosition2D() const {
    return transform;
}


bool Collider::getTrigger() const {
    return isTrigger;
}

float Collider::distanceSquared(const glm::vec2& a, const glm::vec2& b) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}
