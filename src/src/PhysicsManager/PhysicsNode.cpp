#include "include/PhysicsManager/PhysicsNode.hpp"

void PhysicsNode::SetCollider(std::shared_ptr<Collider> col) {
    collider = col;
}

std::shared_ptr<Collider> PhysicsNode::GetCollider() const {
    return collider;
}

void PhysicsNode::setStatic(bool value) {
    isStatic = value;
}

bool PhysicsNode::getStatic() const {
    return isStatic;
}

void PhysicsNode::Update(float dt)
{
    if (isStatic) return;

    // co robiæ jak nie static
}

void PhysicsNode::resolveCollision(const PhysicsNode& other)
{
    if (!collider || !other.collider)
        return;

    if (isStatic && other.isStatic)
        return;


    if (!isStatic)
    {
        velocity = glm::vec2(0.0f, 0.0f);
    }

    // do implementacji

}