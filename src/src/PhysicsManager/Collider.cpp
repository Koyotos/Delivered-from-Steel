#include "include/PhysicsManager/Collider.hpp"

Collider::Collider()
    : isTrigger(false), enabled(true), owner(nullptr) {
}

vector<shared_ptr<Collider>> Collider::getCurrentCollisions() const {
    return currentCollisions;
}

vector<shared_ptr<Collider>> Collider::getPreviousCollisions() const {
    return previousCollisions;
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
