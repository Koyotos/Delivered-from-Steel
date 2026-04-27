#include "include/PhysicsManager/Collider.hpp"
#include "include/PhysicsManager/PhysicsNode.hpp"

Collider::Collider()
    : isTrigger(false), enabled(true) {
	currentCollisions = {};
	previousCollisions = {};
}

std::unordered_set<std::shared_ptr<Collider>>& Collider::GetCurrentCollisions() {
    return currentCollisions;
}

std::unordered_set<std::shared_ptr<Collider>>& Collider::GetPreviousCollisions() {
    return previousCollisions;
}

void Collider::ClearCurrentCollisions() {
	currentCollisions.clear();
}

void Collider::AddToCurrentCollisions(std::shared_ptr<Collider> col) {
	currentCollisions.insert(col);
}

void Collider::SetCurrentToPrevious() {
	previousCollisions = currentCollisions;
	ClearCurrentCollisions();
}

glm::vec2 Collider::GetGlobalPosition2D() const {
    return transform;
}


bool Collider::GetTrigger() const {
    return isTrigger;
}

float Collider::DistanceSquared(const glm::vec2& a, const glm::vec2& b) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

std::shared_ptr<PhysicsNode>& Collider::GetOwner() {
    return owner;
}