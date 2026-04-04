#ifndef FE_PHYSICS_COLLIDER
#define FE_PHYSICS_COLLIDER
#include <glm/matrix.hpp>
#include <memory>
#include <vector>
#include "include/Core/Node.hpp"

class BoxCollider;
class CapsuleCollider;

class Collider
{
private:
    glm::vec2 transform;

    bool isTrigger;
    bool enabled;
    std::shared_ptr<Node> owner;

    vector<shared_ptr<Collider>> currentCollisions;
    vector<shared_ptr<Collider>> previousCollisions;

public:
    Collider();
    virtual ~Collider() = default;

    virtual bool checkCollision(const BoxCollider& other) = 0;
    virtual bool checkCollision(const CapsuleCollider& other) = 0;

	bool getTrigger() const;

    vector<shared_ptr<Collider>> getCurrentCollisions() const;
    vector<shared_ptr<Collider>> getPreviousCollisions() const;
    glm::vec2 getGlobalPosition2D() const;
};

#endif