#include "include/PhysicsManager/PhysicsNode.hpp"
#include <glm/glm.hpp>
#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include <iostream>

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

    // testowa symulacja grawitacji
    velocity.y = std::min(velocity.y + 0.001f * dt, 0.001f);
    Transform t = this->GetTransform(); 
    t.SetTranslation(t.GetTranslation() - glm::vec3(velocity.x, velocity.y, 0.0f));


	this->SetTransform(t);
}

void PhysicsNode::resolveCollision(PhysicsNode& other)
{

    shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();
    
	if (!collider || !other.collider) return;

    if (auto capsule = std::dynamic_pointer_cast<CapsuleCollider>(other.collider)) {
        info = collider->calculateCollisionInfo(capsule);
    }
    else if (auto box = std::dynamic_pointer_cast<BoxCollider>(other.collider)) {
        info = collider->calculateCollisionInfo(box);
    }

    if (!info->collided) return;

	collider->getCurrentCollisions().push_back(other.collider);
    other.collider->getCurrentCollisions().push_back(collider);


    float totalInverseMass = (this->isStatic ? 0.0f : 1.0f) + (other.isStatic ? 0.0f : 1.0f);

    if (totalInverseMass > 0) {
        glm::vec2 separation = info->normal * (info->depth / totalInverseMass);

        std::cout << "Separation: x=" << separation.x
            << ", y=" << separation.y << std::endl;

        if (!this->isStatic) {
            Transform t = this->GetTransform();
            t.SetTranslation(t.GetTranslation() + glm::vec3(separation.x, separation.y, 0.0f));

            this->SetTransform(t);
        }
        if (!this->isStatic) {
            Transform t = other.GetTransform();
            t.SetTranslation(t.GetTranslation() - glm::vec3(separation.x, separation.y, 0.0f));

            other.SetTransform(t);
        }
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
    if (!other.isStatic) {
        other.applyForce(impulse);
    }
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
    velocity = glm::vec2(0.0f, 0.0f);

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