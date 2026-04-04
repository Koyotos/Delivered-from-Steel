#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>
#include "include/PhysicsManager/CapsuleCollider.hpp"

BoxCollider::BoxCollider() : size({ 1.0f, 1.0f }) {}

bool BoxCollider::checkCollision(const BoxCollider& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y);
}

bool BoxCollider::checkCollision(const CapsuleCollider& other) const {
    glm::vec2 closest = {
        other.a.x,
        clamp(boxCenter.y, other.b.y, other.a.y)
    };

    glm::vec2 closestOnBox = {
        clamp(closest.x, min.x, max.x),
        clamp(closest.y, min.y, max.y)
    };

    float distSq = distanceSquared(closest, closestOnBox);
    return distSq <= other.radius * other.radius;
}
