#ifndef FE_PHYSICS_COLLIDER
#define FE_PHYSICS_COLLIDER
#include <glm/matrix.hpp>
#include <memory>
#include <vector>
#include "include/Core/Node.hpp"
#include "include/Core/Transform.hpp"
#include "include/PhysicsManager/CollisionInfo.hpp"

class BoxCollider;
class CapsuleCollider;

class Collider
{
protected:
    glm::vec2 transform;

    bool isTrigger;
    bool enabled;
    std::shared_ptr<Node> owner;

private:
    vector<shared_ptr<Collider>> currentCollisions;
    vector<shared_ptr<Collider>> previousCollisions;
    void clearCurrentCollisions();
public:
    Collider();
    virtual ~Collider() = default;

    virtual bool checkCollision(std::shared_ptr<BoxCollider> other) const = 0;
    virtual bool checkCollision(std::shared_ptr<CapsuleCollider> other) const = 0;

    virtual void updatePosition(const Transform transform) = 0;

    virtual std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<BoxCollider> other) const = 0;
    virtual std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<CapsuleCollider> other) const = 0;

	bool getTrigger() const;

    vector<shared_ptr<Collider>> getCurrentCollisions() const;
    vector<shared_ptr<Collider>> getPreviousCollisions() const;


    void addToCurrentCollisions(shared_ptr<Collider>);
    void setCurrentToPrevious();

    glm::vec2 getGlobalPosition2D() const;

    float distanceSquared(const glm::vec2& a, const glm::vec2& b) const;
};

#endif