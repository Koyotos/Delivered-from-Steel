#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>

CapsuleCollider::CapsuleCollider() : radius(1.0f), height(2.0f) {}

bool CapsuleCollider::checkCollision(const BoxCollider& other) const {
	return other.checkCollision(*this);
}

bool CapsuleCollider::checkCollision(const CapsuleCollider& other) const {
	glm::vec2 closestCapsule = {
	   other.a.x,
	   clamp(b.y, other.b.y, other.a.y)
	};

	glm::vec2 closestCapsuleOther = {
	   a.x,
	   clamp(closestCapsule.y, b.y, a.y)
	};

	float distSq = distanceSquared(closestCapsule, closestCapsuleOther);
	return distSq <= (other.radius + radius) * (other.radius + radius);
	return false;
}