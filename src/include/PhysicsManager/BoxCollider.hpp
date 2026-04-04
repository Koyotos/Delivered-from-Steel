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

    BoxCollider(const Transform transform, float x, float y, float width, float height);

	void updatePosition(const Transform transform) override;

    bool checkCollision(const BoxCollider& other) const override;
    bool checkCollision(const CapsuleCollider& other) const override;

    CollisionInfo calculateCollisionInfo(const BoxCollider& other) const override;
    CollisionInfo calculateCollisionInfo(const CapsuleCollider& other) const override;
};

#endif