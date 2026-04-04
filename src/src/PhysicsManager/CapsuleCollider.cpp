#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>

CapsuleCollider::CapsuleCollider() : radius(1.0f), height(2.0f) {}

bool CapsuleCollider::checkCollision(const BoxCollider& other) const {
	return other.checkCollision(*this);
}

bool CapsuleCollider::checkCollision(const CapsuleCollider& other) const {
    return calculateCollisionInfo(other).collided;
}

CollisionInfo CapsuleCollider::calculateCollisionInfo(const BoxCollider& other) const {
    return other.calculateCollisionInfo(*this);
}

CollisionInfo CapsuleCollider::calculateCollisionInfo(const CapsuleCollider& other) const {
    CollisionInfo info;

	glm::vec2 closestCapsule = {
	   other.a.x,
	   clamp(b.y, other.b.y, other.a.y)
	};

	glm::vec2 closestCapsuleOther = {
	   a.x,
	   clamp(closestCapsule.y, b.y, a.y)
	};

	float distSq = distanceSquared(closestCapsule, closestCapsuleOther);

    if (distSq <= (other.radius + radius) * (other.radius + radius)) {
        info.collided = true;
        float dist = sqrt(distSq);
        info.depth = (other.radius + radius) - dist;
        if (dist > 0) {
            info.normal = (closestCapsule - closestCapsuleOther) / dist;
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