#include "include/PhysicsManager/BoxCollider.hpp"

BoxCollider::BoxCollider() : size({ 1.0f, 1.0f }) {}

bool BoxCollider::checkCollision(const BoxCollider& other) {
    return false;
}

bool BoxCollider::checkCollision(const CapsuleCollider& other) {
    return false;
}