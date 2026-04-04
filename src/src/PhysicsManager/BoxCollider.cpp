#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>
#include "include/PhysicsManager/CapsuleCollider.hpp"

BoxCollider::BoxCollider() : size({ 1.0f, 1.0f }) {}

bool BoxCollider::checkCollision(const BoxCollider& other) const {
    return calculateCollisionInfo(other).collided;
}

bool BoxCollider::checkCollision(const CapsuleCollider& other) const {
    return calculateCollisionInfo(other).collided;
}


CollisionInfo BoxCollider::calculateCollisionInfo(const BoxCollider& other) const {
    CollisionInfo info;

    float overlapX = std::min(max.x, other.max.x) - std::max(min.x, other.min.x);
    float overlapY = std::min(max.y, other.max.y) - std::max(min.y, other.min.y);

    if (overlapX <= 0 || overlapY <= 0) {
        info.collided = false;
        return info;
    }

    info.collided = true;

    if (overlapX < overlapY) {
        info.depth = overlapX;
        info.normal = (boxCenter.x < other.boxCenter.x) ? glm::vec2(-1, 0) : glm::vec2(1, 0);
    }
    else {
        info.depth = overlapY;
        info.normal = (boxCenter.y < other.boxCenter.y) ? glm::vec2(0, -1) : glm::vec2(0, 1);
    }

    return info;
}

CollisionInfo BoxCollider::calculateCollisionInfo(const CapsuleCollider& other) const {
    CollisionInfo info;

    glm::vec2 closest = {
        other.a.x,
        clamp(boxCenter.y, other.b.y, other.a.y)
    };

    glm::vec2 closestOnBox = {
        clamp(closest.x, min.x, max.x),
        clamp(closest.y, min.y, max.y)
    };

    float distSq = distanceSquared(closest, closestOnBox);

    if (distSq <= other.radius * other.radius) {
        info.collided = true;
        float dist = sqrt(distSq);
        info.depth = other.radius - dist;
        if (dist > 0) {
            info.normal = (closest - closestOnBox) / dist;
        }
        else {
            info.normal = glm::vec2(0, 1);
        }
		return info;
    }
    else {
        info.collided = false;
		return info;
    }
}
