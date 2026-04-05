#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>

CapsuleCollider::CapsuleCollider(const Transform transform, float x, float y, float radius, float height)
    : Collider(), radius(radius), height(height)
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

bool CapsuleCollider::checkCollision(std::shared_ptr<BoxCollider> other) const {
	return calculateCollisionInfo(other)->collided;
}

bool CapsuleCollider::checkCollision(std::shared_ptr<CapsuleCollider> other) const {
    return calculateCollisionInfo(other)->collided;
}

std::shared_ptr<CollisionInfo> CapsuleCollider::calculateCollisionInfo(std::shared_ptr<BoxCollider> other) const {
    std::shared_ptr<CollisionInfo> info = other->calculateCollisionInfo(std::const_pointer_cast<CapsuleCollider>(shared_from_this()));
	vec2 tempNormal = info->normal;
	info->normal = -tempNormal;
    return info;
}

std::shared_ptr<CollisionInfo> CapsuleCollider::calculateCollisionInfo(std::shared_ptr<CapsuleCollider> other) const {
    std::shared_ptr<CollisionInfo> info;

	glm::vec2 closestCapsule = {
	   other->a.x,
	   std::clamp(b.y, other->b.y, other->a.y)
	};

	glm::vec2 closestCapsuleOther = {
	   a.x,
       std::clamp(closestCapsule.y, b.y, a.y)
	};

	float distSq = distanceSquared(closestCapsule, closestCapsuleOther);

    if (distSq <= (other->radius + radius) * (other->radius + radius)) {
        info->collided = true;
        float dist = sqrt(distSq);
        info->depth = (other->radius + radius) - dist;
        if (dist > 0) {
            info->normal = (closestCapsule - closestCapsuleOther) / dist;
        }
        else {
            info->normal = glm::vec2(0, 1);
        }
		info->Collider = other;
        return info;
    }
    else {
        info->collided = false;
        return info;
    }
}