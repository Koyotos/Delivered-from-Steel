#ifndef FE_PHYSICS_BOX_COLLIDER
#define FE_PHYSICS_BOX_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
class CapsuleCollider;

class BoxCollider :
    public Collider
{
public:
    glm::vec2 size;

    BoxCollider();

    bool checkCollision(const BoxCollider& other) override;
    bool checkCollision(const CapsuleCollider& other) override;
};

#endif