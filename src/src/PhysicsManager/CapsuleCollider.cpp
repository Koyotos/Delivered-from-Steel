#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include <algorithm>

CapsuleCollider::CapsuleCollider(const Transform transform, float x, float y, float radius, float height)
    : Collider()
{
	this->transform = vec2(x, y);

    this->radius = radius;
    this->height = height;

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
    std::shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();

	glm::vec2 closestCapsule = {
	   other->a.x,
	   std::clamp(b.y, other->b.y, other->a.y)
	};

	glm::vec2 closestCapsuleOther = {
	   a.x,
       std::clamp(closestCapsule.y, b.y, a.y)
	};

	float distSq = distanceSquared(closestCapsule, closestCapsuleOther);

    if (distSq < (other->radius + radius) * (other->radius + radius)) {
        info->collided = true;
        float dist = sqrt(distSq);
        info->depth = (other->radius + radius) - dist;
        if (dist > 0) {
            info->normal = -(closestCapsule - closestCapsuleOther) / dist;
        }
        else {
            info->normal = glm::vec2(0, 1);
        }
		info->collider = other;
        return info;
    }
    else {
        info->collided = false;
        return info;
    }
}

std::optional<float> rayVsCircle(
    const glm::vec2& origin,
    const glm::vec2& dir,
    const glm::vec2& center,
    float radius)
{
    glm::vec2 oc = origin - center;

    float a = glm::dot(dir, dir);
    float b = 2.0f * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return std::nullopt;

    float t = (-b - sqrt(discriminant)) / (2.0f * a);

    if (t < 0)
        return std::nullopt;

    return t;
}

std::optional<RaycastHit> CapsuleCollider::raycast(const glm::vec2& origin, const glm::vec2& dir, float maxDist) {
    float closest = maxDist;
    std::optional<RaycastHit> result;

    if (auto t = rayVsCircle(origin, dir, a, radius))
    {
        if (*t < closest)
        {
            closest = *t;
            result = RaycastHit();
            result->distance = *t;
            result->point = origin + dir * (*t);
            result->normal = glm::normalize(result->point - a);
            result->collider = shared_from_this();
        }
    }

    if (auto t = rayVsCircle(origin, dir, b, radius))
    {
        if (*t < closest)
        {
            closest = *t;
            result = RaycastHit();
            result->distance = *t;
            result->point = origin + dir * (*t);
            result->normal = glm::normalize(result->point - b);
            result->collider = shared_from_this();
        }
    }

    float minX = a.x - radius;
    float maxX = a.x + radius;

    if (dir.x != 0.0f)
    {
        float t1 = (minX - origin.x) / dir.x;
        float t2 = (maxX - origin.x) / dir.x;

        float tNear = std::min(t1, t2);
        float tFar = std::max(t1, t2);

        float t = tNear;

        if (t >= 0 && t < closest)
        {
            float y = origin.y + dir.y * t;

            if (y >= b.y && y <= a.y)
            {
                closest = t;

                result = RaycastHit();
                result->distance = t;
                result->point = origin + dir * t;
                result->normal = (origin.x < a.x) ? glm::vec2(-1, 0) : glm::vec2(1, 0);
                result->collider = shared_from_this();
            }
        }
    }

    return result;
}   