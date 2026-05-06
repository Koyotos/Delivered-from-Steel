#include "include/PhysicsManager/PhysicsNode.hpp"
#include "include/PhysicsManager/PhysicsManager.hpp"

string PhysicsNode::Type() {
    return "PhysicsNode";
}

uint8_t PhysicsNode::RenderType() {
    return 2;
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

    // testowa symulacja grawitacji
    velocity.y = std::clamp(velocity.y - 10.0f * dt, -maxFallSpeed, maxFallSpeed);
    Transform t = this->GetTransform(); 

    t.SetTranslation(t.GetTranslation() + glm::vec3(velocity * dt, 0.0f));

	this->SetTransform(t);
}

void PhysicsNode::resolveCollision(PhysicsNode& other)
{
    shared_ptr<CollisionInfo> info = make_shared<CollisionInfo>();
    
	if (!collider || !other.collider) return;

    if (auto capsule = std::dynamic_pointer_cast<CapsuleCollider>(other.collider)) {
        info = collider->CalculateCollisionInfo(capsule);
    }
    else if (auto box = std::dynamic_pointer_cast<BoxCollider>(other.collider)) {
        info = collider->CalculateCollisionInfo(box);
    }

    if (!info->collided) return;
    if (this->isStatic) return;

	collider->GetCurrentCollisions().insert(other.collider);
	other.collider->GetCurrentCollisions().insert(collider);

    float totalInverseMass = 1;
    //totalInverseMass = (this->isStatic ? 0.0f : 1.0f) + (other.isStatic ? 0.0f : 1.0f);


    glm::vec2 separation = info->normal * (info->depth / totalInverseMass);

    if (!this->isStatic) {
        Transform t = this->GetTransform();
        t.SetTranslation(t.GetTranslation() + glm::vec3(separation.x, separation.y, 0.0f));

        this->SetTransform(t);
        this->ResetGlobal();
        if (collider) collider->UpdatePosition(this->GetTransform());
    }
    //if (!other.isStatic) {
    //    Transform t = other.GetTransform();
    //    t.SetTranslation(t.GetTranslation() - glm::vec3(separation.x, separation.y, 0.0f));

    //    other.SetTransform(t);
    //}
    

    glm::vec2 relativeVelocity = other.velocity - this->velocity;

    float velocityAlongNormal = glm::dot(relativeVelocity, info->normal);

    if (velocityAlongNormal < 0) return;

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
    for (auto& current : collider->GetCurrentCollisions())
    {
        if (collider->GetPreviousCollisions().find(current) == collider->GetPreviousCollisions().end())
        {
            OnCollisionEnter(current);
            OnCollisionStay(current);
        }
        else
        {
            OnCollisionStay(current);
        }
    }

    for (auto& prev : collider->GetPreviousCollisions())
    {
        if (collider->GetCurrentCollisions().find(prev) == collider->GetCurrentCollisions().end())
        {
            OnCollisionExit(prev);
        }
    }
	collider->SetCurrentToPrevious();
}

void PhysicsNode::Init() {
    if (collider) {
        collider->GetOwner() = shared_from_this();
    }
}

std::optional<RaycastHit> PhysicsNode::Raycast(
    const glm::vec2& offset,
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x + offset.x, modelMatrix[3].y + offset.y);

    auto hit = PhysicsManager::GetPhysicsManager().Raycast(origin, direction, maxDistance, collider, type);
    return hit;
}

std::optional<RaycastHit> PhysicsNode::Raycast(
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x, modelMatrix[3].y);

    auto hit = PhysicsManager::GetPhysicsManager().Raycast(origin, direction, maxDistance, collider, type);
    return hit;
}

std::vector<RaycastHit> PhysicsNode::RaycastAll(
    const glm::vec2& offset,
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x + offset.x, modelMatrix[3].y + offset.y);
    auto hits = PhysicsManager::GetPhysicsManager().RaycastAll(origin, direction, maxDistance, collider, type);
    return hits;
}

std::vector<RaycastHit> PhysicsNode::RaycastAll(
    const glm::vec2& direction,
    float maxDistance,
    ObjectType type)
{
    mat4 modelMatrix = GetTransform().GetGlobal();
    vec2 origin = vec2(modelMatrix[3].x, modelMatrix[3].y);
    auto hits = PhysicsManager::GetPhysicsManager().RaycastAll(origin, direction, maxDistance, collider, type);
    return hits;
}