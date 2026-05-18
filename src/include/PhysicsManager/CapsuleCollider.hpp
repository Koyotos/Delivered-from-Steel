#ifndef FE_PHYSICS_CAPSULE_COLLIDER
#define FE_PHYSICS_CAPSULE_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
#include <algorithm>

class CapsuleCollider : public Collider, public enable_shared_from_this<CapsuleCollider> {
    public:
	vec2 a; // one end of the capsule
	vec2 b; // other end of the capsule
    float radius;
    float height;

    uint8_t Type() const noexcept override;

    CapsuleCollider(const Transform transform, float x, float y, float radius, float height);

    void UpdatePosition(const Transform transform) override;

    bool CheckCollision(shared_ptr<Collider> other) const override;

    shared_ptr<CollisionInfo> CalculateCollisionInfo(shared_ptr<Collider> other) const override;

    optional<float> RayVsCircle(const vec2& origin, const vec2& dir, const vec2& center, float radius);

    AABB GetBounds() const override; 

	optional<RaycastHit> Raycast(const vec2& origin, const vec2& dir, float maxDist) override;
};
#endif
