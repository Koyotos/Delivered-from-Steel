#ifndef FE_PHYSICS_CAPSULE_COLLIDER
#define FE_PHYSICS_CAPSULE_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
class BoxCollider;

class CapsuleCollider :
    public Collider, public std::enable_shared_from_this<CapsuleCollider>
{
public:
	glm::vec2 a; // one end of the capsule
	glm::vec2 b; // other end of the capsule
    float radius;
    float height;

    CapsuleCollider(const Transform transform, float x, float y, float radius, float height);

    void updatePosition(const Transform transform) override;

    bool checkCollision(std::shared_ptr<CapsuleCollider> other) const override;
    bool checkCollision(std::shared_ptr<BoxCollider> other) const override;

    std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<BoxCollider> other) const override;
    std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<CapsuleCollider> other) const override;
};
#endif
