#include "include/PhysicsManager/BoxCollider.hpp"

BoxCollider::BoxCollider(const Transform transform, float x, float y, float width, float height)
    : Collider() {

	this->transform = vec2(x, y);
    this->size = vec2(width, height);
    UpdatePosition(transform);
}

void BoxCollider::UpdatePosition(const Transform transform) {
    mat4 modelMatrix = transform.GetGlobal();

    boxCenter = vec2(modelMatrix[3].x + this->transform.x, modelMatrix[3].y + this->transform.y);

    float halfHeight = size.y / 2.0f;
    float halfWidth = size.x / 2.0f;

    vec2 direction = vec2(halfWidth, halfHeight);

    this->max = boxCenter + direction;
    this->min = boxCenter - direction;
}

uint8_t BoxCollider::Type() const noexcept {
    return 1;
}

bool BoxCollider::CheckCollision(shared_ptr<Collider> other) const {
    return CalculateCollisionInfo(other)->collided;
}

shared_ptr<CollisionInfo> BoxCollider::CalculateCollisionInfoB(shared_ptr<BoxCollider> other) const {
    shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();

    float overlapX = std::min(max.x, other->max.x) - std::max(min.x, other->min.x);
    float overlapY = std::min(max.y, other->max.y) - std::max(min.y, other->min.y);

    if (overlapX <= 0 || overlapY <= 0) {
        info->collided = false;
        return info;
    }

    info->collided = true;

    if (overlapX < overlapY) {
        info->depth = overlapX;
        info->normal = (boxCenter.x < other->boxCenter.x) ? vec2(-1, 0) : vec2(1, 0);
    }
    else {
        info->depth = overlapY;
        info->normal = (boxCenter.y < other->boxCenter.y) ? vec2(0, -1) : vec2(0, 1);
    }
    info->collider = other;
    return info;
}

shared_ptr<CollisionInfo> BoxCollider::CalculateCollisionInfoC(shared_ptr<CapsuleCollider> other) const {
    shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();

    vec2 closest = { other->a.x, std::clamp(boxCenter.y, other->b.y, other->a.y)};
    vec2 closestOnBox = {std::clamp(closest.x, min.x, max.x), std::clamp(closest.y, min.y, max.y)};

    float distSq = DistanceSquared(closest, closestOnBox);

    if (distSq <= other->radius * other->radius) {
        info->collided = true;
        float dist = sqrt(distSq);
        if (dist > 0) {
            info->depth = other->radius - dist;
            info->normal = -(closest - closestOnBox) / dist;
        }
        else {
            float distTop = max.y - closest.y;
            float distBottom = closest.y - min.y;
            float distLeft = closest.x - min.x;
            float distRight = max.x - closest.x;

            float minDist = std::min({ distTop, distBottom, distLeft, distRight });

            if (minDist == distTop) info->normal = vec2(0, -1);
            else if (minDist == distBottom) info->normal = vec2(0, 1);
            else if (minDist == distLeft) info->normal = vec2(1, 0);
            else info->normal = vec2(-1, 0);

            info->depth = other->radius + minDist;
        }
        info->collider = other;
		return info;
    }
    else {
        info->collided = false;
		return info;
    }
}

optional<RaycastHit> BoxCollider::Raycast(const vec2& origin, const vec2& dir, float maxDist) {

    vec2 invDir = 1.0f / dir;

    vec2 t1 = (min - origin) * invDir;
    vec2 t2 = (max - origin) * invDir;

    vec2 tmin = glm::min(t1, t2);
    vec2 tmax = glm::max(t1, t2);

    float tNear = std::max(tmin.x, tmin.y);
    float tFar = std::min(tmax.x, tmax.y);

    if (tNear > tFar || tFar < 0 || tNear > maxDist)
        return nullopt;

    optional<RaycastHit> hit = RaycastHit();
    hit->distance = tNear;
    hit->point = origin + dir * tNear;

    if (tmin.x > tmin.y)
        hit->normal = (dir.x < 0) ? vec2(1, 0) : vec2(-1, 0);
    else
        hit->normal = (dir.y < 0) ? vec2(0, 1) : vec2(0, -1);

    hit->collider = shared_from_this();
    return hit;
}

AABB BoxCollider::GetBounds() const {
    vec2 half = size * 0.5f;
    return { boxCenter - half, boxCenter + half };
}

shared_ptr<CollisionInfo> BoxCollider::CalculateCollisionInfo(shared_ptr<Collider> other) const {
    if(other->Type() == 1) {
        return CalculateCollisionInfoB(static_pointer_cast<BoxCollider>(other));
    } else {
        return CalculateCollisionInfoC(static_pointer_cast<CapsuleCollider>(other));
    }
}   