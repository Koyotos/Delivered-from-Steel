#include "include/PhysicsManager/PhysicsNode.hpp"
#include <glm/glm.hpp>
#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"

void PhysicsNode::SetCollider(std::shared_ptr<Collider> col) {
    collider = col;
}

std::shared_ptr<Collider> PhysicsNode::GetCollider() {
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

    shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();
    
    //info = collider->calculateCollisionInfo(other.collider);

    if (!info->collided) return;
	collider->getCurrentCollisions().push_back(other.collider);

    if (this->isStatic && other.isStatic) return;


    if (!info->collided) return;

    float totalInverseMass = (this->isStatic ? 0.0f : 1.0f) + (other.isStatic ? 0.0f : 1.0f);

    if (totalInverseMass > 0) {
        glm::vec2 separation = info->normal * (info->depth / totalInverseMass);

        if (!this->isStatic) {
            // Przesuñ Transform
        }
        //if (!other.isStatic) {
        //    // Przesuñ Transform
        //}
    }

    glm::vec2 relativeVelocity = other.velocity - this->velocity;

    float velocityAlongNormal = glm::dot(relativeVelocity, info->normal);

    if (velocityAlongNormal > 0) return;

    float e = 0.0f;

    float j = -(1.0f + e) * velocityAlongNormal;
    j /= totalInverseMass;

    glm::vec2 impulse = j * info->normal;

    if (!this->isStatic) {
        this->applyForce(impulse);
    }
    //if (!other.isStatic) {
    //    other.applyForce(impulse);
    //}*/
    return;
}

void PhysicsNode::applyForce(const glm::vec2& force) {
    if (isStatic) return;
    velocity += force;
}

PhysicsNode::PhysicsNode() {

}

PhysicsNode::PhysicsNode(const std::unordered_map<std::string, std::any>& data) : VisualNode(data) {
    isStatic = fromMap(bool, "static", data);

    auto it = data.find("colliderPosX");

    float height;
    float x = 0, y = 0;

    auto posX = data.find("colliderPosX");
    if (posX != data.end()) {
		x = fromMap(float, "colliderPosX", data);
    }

    auto posY = data.find("colliderPosY");
    if (posY != data.end()) {
        y = fromMap(float, "colliderPosY", data);
    }

    auto radiusPom = data.find("radius");
    if (radiusPom != data.end()) {
        float radius = fromMap(float, "radius", data);
        float height = fromMap(float, "height", data);

		collider = std::make_shared<CapsuleCollider>(GetTransform(), x, y, radius, height);
        
    }

    auto widthPom = data.find("width");
    if (widthPom != data.end()) {
        float width = fromMap(float, "width", data);
        float height = fromMap(float, "height", data);

        collider = std::make_shared<BoxCollider>(GetTransform(), x, y, width, height);
    }
}