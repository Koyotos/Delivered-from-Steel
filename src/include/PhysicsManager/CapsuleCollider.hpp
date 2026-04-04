#ifndef FE_PHYSICS_CAPSULE_COLLIDER
#define FE_PHYSICS_CAPSULE_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
class BoxCollider;

class CapsuleCollider :
    public Collider
{
public:
	glm::vec2 a; // one end of the capsule
	glm::vec2 b; // other end of the capsule
    float radius;
    float height;

    CapsuleCollider();

    bool checkCollision(const BoxCollider& other) const override;
    bool checkCollision(const CapsuleCollider& other) const override;
};
#endif
