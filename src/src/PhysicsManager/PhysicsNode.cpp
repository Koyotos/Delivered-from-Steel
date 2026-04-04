#include "include/PhysicsManager/PhysicsNode.hpp"
#include <glm/glm.hpp>

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
    /*if (this->isStatic && other.isStatic) return;

    CollisionInfo info = this->GetCollider()->getCollisionData(other.collider.get()); // to inaczej jakoœ bedzie

    if (!info.collided) return;

    float totalInverseMass = (this->isStatic ? 0.0f : 1.0f) + (other.isStatic ? 0.0f : 1.0f);

    if (totalInverseMass > 0) {
        glm::vec2 separation = info.normal * (info.depth / totalInverseMass);

        if (!this->isStatic) {
            // Przesuñ Transform
        }
        //if (!other.isStatic) {
        //    // Przesuñ Transform
        //}
    }

    glm::vec2 relativeVelocity = other.velocity - this->velocity;

    float velocityAlongNormal = glm::dot(relativeVelocity, info.normal);

    if (velocityAlongNormal > 0) return;

    float e = 0.0f;

    float j = -(1.0f + e) * velocityAlongNormal;
    j /= totalInverseMass;

    glm::vec2 impulse = j * info.normal;

    if (!this->isStatic) {
        this->applyForce(impulse);
    }
    //if (!other.isStatic) {
    //    other.applyForce(impulse);
    //}*/

}

void PhysicsNode::applyForce(const glm::vec2& force) {
    if (isStatic) return;
    velocity += force;
}