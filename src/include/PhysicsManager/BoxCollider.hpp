#ifndef FE_PHYSICS_BOX_COLLIDER
#define FE_PHYSICS_BOX_COLLIDER

#include "include/PhysicsManager/Collider.hpp"
class CapsuleCollider;

class BoxCollider :
    public Collider, public std::enable_shared_from_this<BoxCollider>
{
private:
    glm::vec2 min;
	glm::vec2 max;
	glm::vec2 boxCenter;

public:
    glm::vec2 size;

	glm::vec2 getMin() const { return min; }
	glm::vec2 getMax() const { return max; }

    BoxCollider(const Transform transform, float x, float y, float width, float height);

	void updatePosition(const Transform transform) override;

    bool checkCollision(std::shared_ptr<BoxCollider> other) const override;
    bool checkCollision(std::shared_ptr<CapsuleCollider> other) const override;

    std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<BoxCollider> other) const override;
    std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<CapsuleCollider> other) const override;


    std::optional<RaycastHit> raycast(const glm::vec2& origin, const glm::vec2& dir, float maxDist) override;
};

#endif