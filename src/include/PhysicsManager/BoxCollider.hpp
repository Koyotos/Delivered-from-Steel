#ifndef FE_PHYSICS_BOX_COLLIDER
#define FE_PHYSICS_BOX_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
#include "include/PhysicsManager/CapsuleCollider.hpp"
#include <algorithm>

class BoxCollider : public Collider, public enable_shared_from_this<BoxCollider> {
    private:
    vec2 min;
	vec2 max;
	vec2 boxCenter;

    public:
    vec2 size;

	vec2 GetMin() const { return min; }
	vec2 GetMax() const { return max; }

    uint8_t Type() const noexcept override;

    BoxCollider(const Transform transform, float x, float y, float width, float height);

	void UpdatePosition(const Transform transform) override;

    bool CheckCollision(Collider* other) const override;

    shared_ptr<CollisionInfo> CalculateCollisionInfo(Collider* other) const override;

    shared_ptr<CollisionInfo> CalculateCollisionInfoB(BoxCollider* other) const;
    shared_ptr<CollisionInfo> CalculateCollisionInfoC(CapsuleCollider* other) const;

    AABB GetBounds() const override;

    optional<RaycastHit> Raycast(const vec2& origin, const vec2& dir, float maxDist) override;
};

#endif