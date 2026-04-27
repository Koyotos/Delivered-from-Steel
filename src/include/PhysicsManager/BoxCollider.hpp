#ifndef FE_PHYSICS_BOX_COLLIDER
#define FE_PHYSICS_BOX_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
#include "include/PhysicsManager/CapsuleCollider.hpp"
#include <algorithm>

class CapsuleCollider;

class BoxCollider : public Collider, public enable_shared_from_this<BoxCollider> {
    private:
    vec2 min;
	vec2 max;
	vec2 boxCenter;

    public:
    vec2 size;

	vec2 getMin() const { return min; }
	vec2 getMax() const { return max; }

    BoxCollider(const Transform transform, float x, float y, float width, float height);

	void UpdatePosition(const Transform transform) override;

    bool CheckCollision(shared_ptr<BoxCollider> other) const override;
    bool CheckCollision(shared_ptr<CapsuleCollider> other) const override;

    shared_ptr<CollisionInfo> CalculateCollisionInfo(shared_ptr<BoxCollider> other) const override;
    shared_ptr<CollisionInfo> CalculateCollisionInfo(shared_ptr<CapsuleCollider> other) const override;


    optional<RaycastHit> Raycast(const vec2& origin, const vec2& dir, float maxDist) override;
};

#endif