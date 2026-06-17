#include "include/PhysicsManager/Collider.hpp"

Collider::Collider() : isTrigger(false), enabled(true) {
	currentCollisions = {};
	previousCollisions = {};
}

unordered_set<shared_ptr<Collider>>& Collider::GetCurrentCollisions() {
    return currentCollisions;
}

unordered_set<shared_ptr<Collider>>& Collider::GetPreviousCollisions() {
    return previousCollisions;
}

void Collider::ClearCurrentCollisions() {
	currentCollisions.clear();
}

void Collider::AddToCurrentCollisions(shared_ptr<Collider> col) {
	currentCollisions.insert(col);
}

void Collider::SetCurrentToPrevious() {
	previousCollisions = currentCollisions;
	ClearCurrentCollisions();
}

vec2 Collider::GetGlobalPosition2D() const {
    return transform;
}

uint8_t Collider::Type() const noexcept {
    return 0;
}

bool Collider::GetTrigger() const {
    return isTrigger;
}

float Collider::DistanceSquared(const vec2& a, const vec2& b) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

shared_ptr<PhysicsNode> Collider::GetOwner() {
    if (auto ptr = owner.lock()) {
        return ptr;
    }
    return nullptr;
}

void Collider::SetOwner(std::shared_ptr<PhysicsNode> node) {
	owner = node;
}