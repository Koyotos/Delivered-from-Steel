#ifndef FE_PHYSICS_NODE
#define FE_PHYSICS_NODE
#include <memory>
#include <glm/matrix.hpp>
#include "include/PhysicsManager/Collider.hpp"
#include "include/Core/VisualNode.hpp"

enum class ObjectType {
	Default,
    Player,
    Enemy,
    Wall
};

class PhysicsNode : public VisualNode, public std::enable_shared_from_this<PhysicsNode>
{
private:
	bool isStatic = false;

    std::shared_ptr<Collider> collider;
    glm::vec2 velocity;

	std::shared_ptr<Shader> debugShader;

protected:

    ObjectType objectType = ObjectType::Default;

public:
	float maxFallSpeed = 50.0f;

    string Type() override;

    void SetCollider(std::shared_ptr<Collider> col);
    std::shared_ptr<Collider> GetCollider();

    void setStatic(bool value);
    bool getStatic() const;

    void Update(float dt);

    void resolveCollision(PhysicsNode& other);

    void applyForce(const glm::vec2& force);

	void drawDebug();
    void drawBox();
    void drawCapsule();

	void setDebugShader(std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> getDebugShader() const { return debugShader; };

    virtual void OnCollisionEnter(std::shared_ptr<Collider> other) {}
    virtual void OnCollisionStay(std::shared_ptr<Collider> other) {}
    virtual void OnCollisionExit(std::shared_ptr<Collider> other) {}

    void processCollisions();

    void Init();

	ObjectType GetObjectType() const { return objectType; }

    PhysicsNode();

    PhysicsNode(const std::unordered_map<std::string, std::any>&);
};

#endif