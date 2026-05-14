#include "include/PhysicsManager/BoxCollider.hpp"

BoxCollider::BoxCollider(const Transform transform, float x, float y, float width, float height)
    : Collider()
{
	this->transform = vec2(x, y);

    this->size = vec2(width, height);

    UpdatePosition(transform);
}


void BoxCollider::UpdatePosition(const Transform transform)
{
    mat4 modelMatrix = transform.GetGlobal();

    boxCenter = vec2(modelMatrix[3].x + this->transform.x, modelMatrix[3].y + this->transform.y);

    float halfHeight = size.y / 2.0f;
    float halfWidth = size.x / 2.0f;

    vec2 direction = vec2(halfWidth, halfHeight);

    this->max = boxCenter + direction;
    this->min = boxCenter - direction;
}


bool BoxCollider::CheckCollision(std::shared_ptr<BoxCollider> other) const {
    return CalculateCollisionInfo(other)->collided;
}

bool BoxCollider::CheckCollision(std::shared_ptr<CapsuleCollider> other) const {
    return CalculateCollisionInfo(other)->collided;
}

std::shared_ptr<CollisionInfo> BoxCollider::CalculateCollisionInfo(std::shared_ptr<BoxCollider> other) const {
    std::shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();

    float overlapX = std::min(max.x, other->max.x) - std::max(min.x, other->min.x);
    float overlapY = std::min(max.y, other->max.y) - std::max(min.y, other->min.y);

    if (overlapX <= 0 || overlapY <= 0) {
        info->collided = false;
        return info;
    }

    info->collided = true;

    if (overlapX < overlapY) {
        info->depth = overlapX;
        info->normal = (boxCenter.x < other->boxCenter.x) ? glm::vec2(-1, 0) : glm::vec2(1, 0);
    }
    else {
        info->depth = overlapY;
        info->normal = (boxCenter.y < other->boxCenter.y) ? glm::vec2(0, -1) : glm::vec2(0, 1);
    }
    info->collider = other;
    return info;
}

std::shared_ptr<CollisionInfo> BoxCollider::CalculateCollisionInfo(std::shared_ptr<CapsuleCollider> other) const {
    std::shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();

    glm::vec2 closest = {
        other->a.x,
        std::clamp(boxCenter.y, other->b.y, other->a.y)
    };

    glm::vec2 closestOnBox = {
        std::clamp(closest.x, min.x, max.x),
        std::clamp(closest.y, min.y, max.y)
    };

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

            if (minDist == distTop) info->normal = glm::vec2(0, -1);
            else if (minDist == distBottom) info->normal = glm::vec2(0, 1);
            else if (minDist == distLeft) info->normal = glm::vec2(1, 0);
            else info->normal = glm::vec2(-1, 0);

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

std::optional<RaycastHit> BoxCollider::Raycast(const glm::vec2& origin, const glm::vec2& dir, float maxDist) {

    glm::vec2 invDir = 1.0f / dir;

    glm::vec2 t1 = (min - origin) * invDir;
    glm::vec2 t2 = (max - origin) * invDir;

    glm::vec2 tmin = glm::min(t1, t2);
    glm::vec2 tmax = glm::max(t1, t2);

    float tNear = std::max(tmin.x, tmin.y);
    float tFar = std::min(tmax.x, tmax.y);

    if (tNear > tFar || tFar < 0 || tNear > maxDist)
        return std::nullopt;

    optional<RaycastHit> hit = RaycastHit();
    hit->distance = tNear;
    hit->point = origin + dir * tNear;

    if (tmin.x > tmin.y)
        hit->normal = (dir.x < 0) ? glm::vec2(1, 0) : glm::vec2(-1, 0);
    else
        hit->normal = (dir.y < 0) ? glm::vec2(0, 1) : glm::vec2(0, -1);

    hit->collider = shared_from_this();
    return hit;
}

AABB BoxCollider::GetBounds() const {
    vec2 half = size * 0.5f;
    return { boxCenter - half, boxCenter + half };
}
