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
    vec2 transform;
    bool isTrigger;
    bool enabled;
    shared_ptr<PhysicsNode> owner;

    private:
    unordered_set<shared_ptr<Collider>> currentCollisions;
    unordered_set<shared_ptr<Collider>> previousCollisions;
    void ClearCurrentCollisions();

    public:
    Collider();
    virtual ~Collider() = default;

    virtual bool CheckCollision(shared_ptr<BoxCollider> other) const = 0;
    virtual bool CheckCollision(shared_ptr<CapsuleCollider> other) const = 0;

    virtual void UpdatePosition(const Transform transform) = 0;

    virtual shared_ptr<CollisionInfo> CalculateCollisionInfo(shared_ptr<BoxCollider> other) const = 0;
    virtual shared_ptr<CollisionInfo> CalculateCollisionInfo(shared_ptr<CapsuleCollider> other) const = 0;

    virtual optional<RaycastHit> Raycast(const vec2& origin, const vec2& dir, float maxDist) = 0;

	bool GetTrigger() const;

    unordered_set<shared_ptr<Collider>>& GetCurrentCollisions();
    unordered_set<shared_ptr<Collider>>& GetPreviousCollisions();


    void AddToCurrentCollisions(shared_ptr<Collider>);
    void SetCurrentToPrevious();

    vec2 GetGlobalPosition2D() const;

    float DistanceSquared(const vec2& a, const vec2& b) const;

    shared_ptr<PhysicsNode>& GetOwner();

};

#endif