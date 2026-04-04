#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>
#include "include/PhysicsManager/CapsuleCollider.hpp"

BoxCollider::BoxCollider(const Transform transform, float x, float y, float width, float height)
    : size((width, height))
{
	this->transform = vec2(x, y);

    updatePosition(transform);
}


void BoxCollider::updatePosition(const Transform transform)
{
    mat4 modelMatrix = transform.GetGlobal();

    boxCenter = vec2(modelMatrix[3].x + this->transform.x, modelMatrix[3].y + this->transform.y);

    float halfHeight = size.y / 2.0f;
    float halfWidth = size.x / 2.0f;

    vec2 direction = vec2(halfWidth, halfHeight);

    this->max = boxCenter + direction;
    this->min = boxCenter - direction;
}


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
        std::clamp(boxCenter.y, other.b.y, other.a.y)
    };

    glm::vec2 closestOnBox = {
        std::clamp(closest.x, min.x, max.x),
        std::clamp(closest.y, min.y, max.y)
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
