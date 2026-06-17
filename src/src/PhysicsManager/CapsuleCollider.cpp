#include "include/PhysicsManager/CapsuleCollider.hpp"

CapsuleCollider::CapsuleCollider(const Transform transform, float x, float y, float radius, float height)
    : Collider() {
	this->transform = vec2(x, y);

    this->radius = radius;
    this->height = height;

    UpdatePosition(transform);
}

void CapsuleCollider::UpdatePosition(const Transform transform) {
    mat4 modelMatrix = transform.GetGlobal();

    vec2 center = vec2(modelMatrix[3].x + this->transform.x, modelMatrix[3].y + this->transform.y);

    vec2 upDirection = vec2(0, 1);

    float segmentHalfLength = height / 2.0f;

    this->a = center + upDirection * segmentHalfLength;
    this->b = center - upDirection * segmentHalfLength;
}

bool CapsuleCollider::CheckCollision(Collider* other) const {
    return CalculateCollisionInfo(other)->collided;
}

uint8_t CapsuleCollider::Type() const noexcept {
    return 2;
}

shared_ptr<CollisionInfo> CapsuleCollider::CalculateCollisionInfo(Collider* other) const {
    if(other->Type() == 1) {
        shared_ptr<CollisionInfo> info = other->CalculateCollisionInfo(const_cast<CapsuleCollider*>(this));
        vec2 tempNormal = info->normal;
        info->normal = -tempNormal;
        return info;
    }
    CapsuleCollider* capsule = static_cast<CapsuleCollider*>(other);
    shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();

	vec2 closestCapsule = { capsule->a.x, std::clamp(b.y, capsule->b.y, capsule->a.y) };
	vec2 closestCapsuleOther = { a.x, std::clamp(closestCapsule.y, b.y, a.y) };

	float distSq = DistanceSquared(closestCapsule, closestCapsuleOther);

    if(distSq < (capsule->radius + radius) * (capsule->radius + radius)) {
        info->collided = true;
        float dist = sqrt(distSq);
        info->depth = (capsule->radius + radius) - dist;
        if (dist > 0) {
            info->normal = -(closestCapsule - closestCapsuleOther) / dist;
        }
        else {
            info->normal = vec2(0, 1);
        }
		info->collider = capsule;
        return info;
    }
    else {
        info->collided = false;
        return info;
    }
}

optional<float> CapsuleCollider::RayVsCircle(const vec2& origin, const vec2& dir, const vec2& center, float radius) {
    vec2 oc = origin - center;

    float a = glm::dot(dir, dir);
    float b = 2.0f * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return nullopt;

    float t = (-b - sqrt(discriminant)) / (2.0f * a);

    if (t < 0)
        return nullopt;

    return t;
}

optional<RaycastHit> CapsuleCollider::Raycast(const vec2& origin, const vec2& dir, float maxDist) {
    float closest = maxDist;
    optional<RaycastHit> result;

    if (auto t = RayVsCircle(origin, dir, a, radius)) {
        if (*t < closest) {
            closest = *t;
            result = RaycastHit();
            result->distance = *t;
            result->point = origin + dir * (*t);
            result->normal = normalize(result->point - a);
            result->collider = shared_from_this();
        }
    }

    if (auto t = RayVsCircle(origin, dir, b, radius)) {
        if (*t < closest) {
            closest = *t;
            result = RaycastHit();
            result->distance = *t;
            result->point = origin + dir * (*t);
            result->normal = normalize(result->point - b);
            result->collider = shared_from_this();
        }
    }
    float minX = a.x - radius;
    float maxX = a.x + radius;
    if (dir.x != 0.0f) {
        float t1 = (minX - origin.x) / dir.x;
        float t2 = (maxX - origin.x) / dir.x;

        float tNear = std::min(t1, t2);
        float tFar = std::max(t1, t2);

        float t = tNear;

        if (t >= 0 && t < closest) {
            float y = origin.y + dir.y * t;

            if (y >= b.y && y <= a.y) {
                closest = t;

                result = RaycastHit();
                result->distance = t;
                result->point = origin + dir * t;
                result->normal = (origin.x < a.x) ? vec2(-1, 0) : vec2(1, 0);
                result->collider = shared_from_this();
            }
        }
    }

    return result;
}

AABB CapsuleCollider::GetBounds() const {
    vec2 center = (a + b) * 0.5f;
    vec2 extents = vec2(radius, height * 0.5f + radius);
    return { center - extents, center + extents };
}
