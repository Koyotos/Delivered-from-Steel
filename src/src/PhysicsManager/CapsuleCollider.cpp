#include "include/PhysicsManager/CapsuleCollider.hpp"

CapsuleCollider::CapsuleCollider() : radius(1.0f), height(2.0f) {}

bool CapsuleCollider::checkCollision(const BoxCollider& other) {
    return false;
}

bool CapsuleCollider::checkCollision(const CapsuleCollider& other) {
    return false;
}