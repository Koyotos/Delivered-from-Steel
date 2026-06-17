#include "include/PhysicsManager/PhysicsNode.hpp"
#include "include/PhysicsManager/PhysicsManager.hpp"


string PhysicsNode::Type() {
    return "PhysicsNode";
}

uint8_t PhysicsNode::RenderType() {
    return 2;
}

void PhysicsNode::SetCollider(shared_ptr<Collider> col) {
    collider = col;
}

shared_ptr<Collider> PhysicsNode::GetCollider() {
    return collider;
}

void PhysicsNode::SetStatic(bool value) {
    isStatic = value;
}

bool PhysicsNode::GetStatic() const {
    return isStatic;
}

void PhysicsNode::Physics(const float& dt) {
    if (isStatic) return;

    // testowa symulacja grawitacji
    velocity.y = std::clamp(velocity.y - 10.0f * dt, -maxFallSpeed, maxFallSpeed);
    Transform t = this->GetTransform(); 

    t.SetTranslation(t.GetTranslation() + vec3(velocity * dt, 0.0f));

	this->SetTransform(t);
}

shared_ptr<CollisionInfo> PhysicsNode::GetCollisionInfo(shared_ptr<Collider> other) {
    if (!collider || !other) return nullptr;
    if (other->Type() == 2) {
        auto capsule = static_pointer_cast<CapsuleCollider>(other);
        return collider->CalculateCollisionInfo(capsule);
    }
    else if (other->Type() == 1) {
        auto box = static_pointer_cast<BoxCollider>(other);
        return collider->CalculateCollisionInfo(box);
    }
    return nullptr;
}

void PhysicsNode::ResolveCollision(PhysicsNode& other) {
    shared_ptr<CollisionInfo> info = GetCollisionInfo(other.GetCollider());
    
	if (!info) return;
    if (!info->collided) return;
    if (this->isStatic) return;

	collider->GetCurrentCollisions().insert(other.collider);
	other.collider->GetCurrentCollisions().insert(collider);

    vec2 separation = info->normal * info->depth;

    if (!this->isStatic && isResolveCollision && other.isResolveCollision) {
        Transform t = this->GetTransform();
        t.SetTranslation(t.GetTranslation() + vec3(separation.x, separation.y, 0.0f));

        this->SetTransform(t);
        this->ResetGlobal();
        if (collider) collider->UpdatePosition(this->GetTransform());
    }

    vec2 relativeVelocity = other.velocity - this->velocity;

    float velocityAlongNormal = dot(relativeVelocity, info->normal);

    if (velocityAlongNormal < 0) return;

    float j = -velocityAlongNormal;

    vec2 impulse = j * info->normal;

    if (!this->isStatic && isResolveCollision && other.isResolveCollision) {
        this->ApplyForce(-impulse);
    }
    return;
}

void PhysicsNode::ApplyForce(const vec2& force) {
    if (isStatic) return;
    velocity += force;
}

PhysicsNode::PhysicsNode() {

}

PhysicsNode::PhysicsNode(const std::unordered_map<std::string, std::any>& data) : VisualNode(data) {
    isStatic = fromMap(bool, "static", data);
    isResolveCollision = fromMap(bool, "ResolveCollision", data);
    velocity = vec2(0.0f, 0.0f);

    bool addCollider = fromMap(bool, "addCollider", data);
    if (!addCollider) return;

    float x = fromMap(float, "colliderPosX",data);
    float y = fromMap(float, "colliderPosY",data);

    bool colliderType = fromMap(bool,"colliderType",data);
    if(colliderType) {
        float radius = fromMap(float, "radius", data);
        float height = fromMap(float, "height", data);
		collider = make_shared<CapsuleCollider>(GetTransform(), x, y, radius, height);
        
    } else {
        float width = fromMap(float, "width", data);
        float height = fromMap(float, "height", data);
        collider = make_shared<BoxCollider>(GetTransform(), x, y, width, height);
    }
    
}

void PhysicsNode::SetDebugShader(shared_ptr<Shader> shader) {
    debugShader = shader;
}

void PhysicsNode::DrawCapsule() {
    return;
}

void PhysicsNode::DrawBox() {
    auto box = dynamic_pointer_cast<BoxCollider>(collider);
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

void PhysicsNode::processCollisions() {
    for (auto& current : collider->GetCurrentCollisions()) {
        if (collider->GetPreviousCollisions().find(current) == collider->GetPreviousCollisions().end()) {
            OnCollisionEnter(current);
            OnCollisionStay(current);
        }
        else {
            OnCollisionStay(current);
        }
    }

    for (auto& prev : collider->GetPreviousCollisions()) {
        if (collider->GetCurrentCollisions().find(prev) == collider->GetCurrentCollisions().end()) {
            OnCollisionExit(prev);
        }
    }
	collider->SetCurrentToPrevious();
}

void PhysicsNode::Init() {
    if (collider) {
        collider->SetOwner(shared_from_this());
    }
}

optional<RaycastHit> PhysicsNode::Raycast(const vec2& offset, const vec2& direction,
    float maxDistance, uint32_t type) {

    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x + offset.x, modelMatrix[3].y + offset.y);

    auto hit = PhysicsManager::GetPhysicsManager().Raycast(origin, direction, maxDistance, collider, type);
    return hit;
}

optional<RaycastHit> PhysicsNode::Raycast( const vec2& direction, float maxDistance,
    uint32_t type) {

    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x, modelMatrix[3].y);

    auto hit = PhysicsManager::GetPhysicsManager().Raycast(origin, direction, maxDistance, collider, type);
    return hit;
}

vector<RaycastHit> PhysicsNode::RaycastAll(const vec2& offset, const vec2& direction,
    float maxDistance, uint32_t type) {

    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x + offset.x, modelMatrix[3].y + offset.y);
    auto hits = PhysicsManager::GetPhysicsManager().RaycastAll(origin, direction, maxDistance, collider, type);
    return hits;
}

vector<RaycastHit> PhysicsNode::RaycastAll(const vec2& direction, float maxDistance,
    uint32_t type) {

    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x, modelMatrix[3].y);
    auto hits = PhysicsManager::GetPhysicsManager().RaycastAll(origin, direction, maxDistance, collider, type);
    return hits;
}