#include "include/PhysicsManager/PhysicsNode.hpp"
#include <glm/glm.hpp>
#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include <iostream>
#include "include/PhysicsManager/PhysicsManager.hpp"
string PhysicsNode::Type() {
    return "PhysicsNode";
}

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

	float deltaTime = std::min(dt, 0.016f);

    // testowa symulacja grawitacji
    velocity.y = std::clamp(velocity.y - 10.0f * deltaTime, -maxFallSpeed, maxFallSpeed);
    Transform t = this->GetTransform(); 

    t.SetTranslation(t.GetTranslation() + glm::vec3(velocity * deltaTime, 0.0f));

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
    if (this->isStatic) return;

	collider->getCurrentCollisions().insert(other.collider);
	other.collider->getCurrentCollisions().insert(collider);

    float totalInverseMass = 1;
    //totalInverseMass = (this->isStatic ? 0.0f : 1.0f) + (other.isStatic ? 0.0f : 1.0f);


    glm::vec2 separation = info->normal * (info->depth / totalInverseMass);

    if (!this->isStatic) {
        Transform t = this->GetTransform();
        t.SetTranslation(t.GetTranslation() + glm::vec3(separation.x, separation.y, 0.0f));

        this->SetTransform(t);
    }
    //if (!other.isStatic) {
    //    Transform t = other.GetTransform();
    //    t.SetTranslation(t.GetTranslation() - glm::vec3(separation.x, separation.y, 0.0f));

    //    other.SetTransform(t);
    //}
    

    glm::vec2 relativeVelocity = other.velocity - this->velocity;

    float velocityAlongNormal = glm::dot(relativeVelocity, info->normal);


    float e = 0.0f;

    float j = -(1.0f + e) * velocityAlongNormal;
    j /= totalInverseMass;

    glm::vec2 impulse = j * info->normal;

    if (!this->isStatic) {
        this->applyForce(-impulse);
    }
    //if (!other.isStatic) {
    //    other.applyForce(impulse);
    //}
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

    bool addCollider = fromMap(bool, "addCollider", data);
    if (!addCollider) return;

    float x = fromMap(float, "colliderPosX",data);
    float y = fromMap(float, "colliderPosY",data);

    bool colliderType = fromMap(bool,"colliderType",data);
    if(colliderType) {
        float radius = fromMap(float, "radius", data);
        float height = fromMap(float, "height", data);
		collider = std::make_shared<CapsuleCollider>(GetTransform(), x, y, radius, height);
        
    } else {
        float width = fromMap(float, "width", data);
        float height = fromMap(float, "height", data);
        collider = std::make_shared<BoxCollider>(GetTransform(), x, y, width, height);
    }
    
}

void PhysicsNode::drawDebug() {
    #if defined(DEBUG)
    if (collider) {
		shader->Use();
        shader->SetBool("isDebug", true);
        if (auto capsule = std::dynamic_pointer_cast<CapsuleCollider>(collider)) {
            drawCapsule();
        }
        else if (auto box = std::dynamic_pointer_cast<BoxCollider>(collider)) {
            drawBox();
        }
		shader->SetBool("isDebug", false);

    }
    #endif
}

void PhysicsNode::setDebugShader(std::shared_ptr<Shader> shader) {
    debugShader = shader;
}

void PhysicsNode::drawCapsule() {
    return;
}

void PhysicsNode::drawBox() {
    auto box = std::dynamic_pointer_cast<BoxCollider>(collider);
    if (!box) return;

    float halfW = box->size.x / 2.0f;
    float halfH = box->size.y / 2.0f;
    float vertices[] = {
        -halfW, -halfH,  halfW, -halfH,
         halfW, -halfH,  halfW,  halfH,
         halfW,  halfH, -halfW,  halfH,
        -halfW,  halfH, -halfW, -halfH
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 8);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void PhysicsNode::processCollisions()
{
    for (auto& current : collider->getCurrentCollisions())
    {
        if (collider->getPreviousCollisions().find(current) == collider->getPreviousCollisions().end())
        {
            OnCollisionEnter(current);
        }
        else
        {
            OnCollisionStay(current);
        }
    }

    for (auto& prev : collider->getPreviousCollisions())
    {
        if (collider->getCurrentCollisions().find(prev) == collider->getCurrentCollisions().end())
        {
            OnCollisionExit(prev);
        }
    }
	collider->setCurrentToPrevious();
}

void PhysicsNode::Init() {
    if (collider) {
        collider->getOwner() = shared_from_this();
    }
}

std::optional<RaycastHit> PhysicsNode::raycast(
    const glm::vec2& offset,
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x + offset.x, modelMatrix[3].y + offset.y);

    auto hit = PhysicsManager::GetPhysicsManager().raycast(origin, direction, maxDistance, collider, type);
    return hit;
}

std::optional<RaycastHit> PhysicsNode::raycast(
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x, modelMatrix[3].y);

    auto hit = PhysicsManager::GetPhysicsManager().raycast(origin, direction, maxDistance, collider, type);
    return hit;
}

std::vector<RaycastHit> PhysicsNode::raycastAll(
    const glm::vec2& offset,
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x + offset.x, modelMatrix[3].y + offset.y);
    auto hits = PhysicsManager::GetPhysicsManager().raycastAll(origin, direction, maxDistance, collider, type);
    return hits;
}

std::vector<RaycastHit> PhysicsNode::raycastAll(
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x, modelMatrix[3].y);
    auto hits = PhysicsManager::GetPhysicsManager().raycastAll(origin, direction, maxDistance, collider, type);
    return hits;
}