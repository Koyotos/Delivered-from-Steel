#ifndef FE_PHYSICS_BOX_COLLIDER
#define FE_PHYSICS_BOX_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
class CapsuleCollider;

class BoxCollider :
    public Collider
{
private:
    glm::vec2 min;
	glm::vec2 max;
	glm::vec2 boxCenter;

public:
    glm::vec2 size;

    BoxCollider();

    bool checkCollision(const BoxCollider& other) override;
    bool checkCollision(const CapsuleCollider& other) override;
};

#endif