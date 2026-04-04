#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>
#include "include/PhysicsManager/CapsuleCollider.hpp"

BoxCollider::BoxCollider() : size({ 1.0f, 1.0f }) {}

bool BoxCollider::checkCollision(const BoxCollider& other) {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y);
}

float distanceSquared(const glm::vec2& a, const glm::vec2& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

bool BoxCollider::checkCollision(const CapsuleCollider& other) {
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
