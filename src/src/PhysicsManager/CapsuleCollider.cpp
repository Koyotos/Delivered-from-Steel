#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>

CapsuleCollider::CapsuleCollider(const Transform transform, float x, float y, float radius, float height)
    : radius(radius), height(height)
{
	this->transform = vec2(x, y);

    updatePosition(transform);
}

void CapsuleCollider::updatePosition(const Transform transform)
{
    mat4 modelMatrix = transform.GetGlobal();

    vec2 center = vec2(modelMatrix[3].x + this->transform.x, modelMatrix[3].y + this->transform.y);

    vec2 upDirection = vec2(0, 1);

    float segmentHalfLength = height / 2.0f;

    this->a = center + upDirection * segmentHalfLength;
    this->b = center - upDirection * segmentHalfLength;
}

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
	   std::clamp(b.y, other.b.y, other.a.y)
	};

	glm::vec2 closestCapsuleOther = {
	   a.x,
       std::clamp(closestCapsule.y, b.y, a.y)
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