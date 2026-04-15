#ifndef FE_PHYSICS_COLLIDER
#define FE_PHYSICS_COLLIDER
#include <glm/matrix.hpp>
#include <memory>
#include <unordered_set>
#include "include/Core/Node.hpp"
#include "include/Core/Transform.hpp"
#include "include/PhysicsManager/CollisionInfo.hpp"
#include <optional>
#include "include/PhysicsManager/RaycastHit.hpp"

class BoxCollider;
class CapsuleCollider;

class PhysicsNode;

class Collider
{
protected:
    glm::vec2 transform;

    bool isTrigger;
    bool enabled;
    std::shared_ptr<PhysicsNode> owner;

private:
    std::unordered_set<std::shared_ptr<Collider>> currentCollisions;
    std::unordered_set<std::shared_ptr<Collider>> previousCollisions;
    void clearCurrentCollisions();
public:
    Collider();
    virtual ~Collider() = default;

    virtual bool checkCollision(std::shared_ptr<BoxCollider> other) const = 0;
    virtual bool checkCollision(std::shared_ptr<CapsuleCollider> other) const = 0;

    virtual void updatePosition(const Transform transform) = 0;

    virtual std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<BoxCollider> other) const = 0;
    virtual std::shared_ptr<CollisionInfo> calculateCollisionInfo(std::shared_ptr<CapsuleCollider> other) const = 0;

    virtual std::optional<RaycastHit> raycast(const glm::vec2& origin, const glm::vec2& dir, float maxDist) = 0;

	bool getTrigger() const;

    std::unordered_set<std::shared_ptr<Collider>>& getCurrentCollisions();
    std::unordered_set<std::shared_ptr<Collider>>& getPreviousCollisions();


    void addToCurrentCollisions(shared_ptr<Collider>);
    void setCurrentToPrevious();

    glm::vec2 getGlobalPosition2D() const;

    float distanceSquared(const glm::vec2& a, const glm::vec2& b) const;

    std::shared_ptr<PhysicsNode>& getOwner();

};

#endif