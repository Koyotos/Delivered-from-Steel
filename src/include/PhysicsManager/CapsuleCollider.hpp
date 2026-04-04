#ifndef FE_PHYSICS_CAPSULE_COLLIDER
#define FE_PHYSICS_CAPSULE_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
class BoxCollider;

class CapsuleCollider :
    public Collider
{
public:
    float radius;
    float height;

    CapsuleCollider();

    bool checkCollision(const BoxCollider& other) override;
    bool checkCollision(const CapsuleCollider& other) override;
};
#endif
