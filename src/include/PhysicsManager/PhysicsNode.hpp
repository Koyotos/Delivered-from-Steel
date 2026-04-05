#ifndef FE_PHYSICS_NODE
#define FE_PHYSICS_NODE

#include <memory>
#include <glm/matrix.hpp>
#include "include/PhysicsManager/Collider.hpp"
#include "include/Core/VisualNode.hpp"


class PhysicsNode : public VisualNode
{
private:
	bool isStatic = false;

    std::shared_ptr<Collider> collider;
    glm::vec2 velocity;

public:
	float maxFallSpeed = 5.0f;

    void SetCollider(std::shared_ptr<Collider> col);
    std::shared_ptr<Collider> GetCollider();

    void setStatic(bool value);
    bool getStatic() const;

    void Update(float dt);

    void resolveCollision(PhysicsNode& other);

    void applyForce(const glm::vec2& force);

    PhysicsNode();

    PhysicsNode(const std::unordered_map<std::string, std::any>&);
};

#endif