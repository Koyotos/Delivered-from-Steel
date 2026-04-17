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
    Wall,
    Null
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

    virtual void Update(float dt);

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

    glm::vec2 GetVelocity() const { return velocity; }
    void SetVelocity(const glm::vec2& v) { velocity = v; }

    void processCollisions();

    void Init();

	ObjectType GetObjectType() const { return objectType; }

    PhysicsNode();

    PhysicsNode(const std::unordered_map<std::string, std::any>&);

    /**
     * Performs a raycast from a world-space position with an explicit offset.
     *
     * @param offset Local-space offset added to the node's world position.
     * @param direction Direction of the ray in world space (should be normalized).
     * @param maxDistance Maximum distance the ray can travel.
     * @return RaycastHit if an object is hit, otherwise std::nullopt.
     */

    std::optional<RaycastHit> raycast(const glm::vec2&, const glm::vec2&, float, ObjectType type = ObjectType::Null);

    /**
    * Performs a raycast from a world-space position.
    *
    * @param direction Direction of the ray in world space (should be normalized).
    * @param maxDistance Maximum distance the ray can travel.
    * @return RaycastHit if an object is hit, otherwise std::nullopt.
    */
    std::optional<RaycastHit> raycast(const glm::vec2&, float, ObjectType type = ObjectType::Null);

    /**
     * Performs a raycast that returns all hits along the ray from a world-space position with an explicit offset.
     *
     * @param offset Local-space offset added to the node's world position.
     * @param direction Direction of the ray in world space (should be normalized).
     * @param maxDistance Maximum distance the ray can travel.
     * @param type Filter for object types to include in the raycast.
     * @return A vector of all RaycastHit results sorted by distance (if implemented that way).
     */
    std::vector<RaycastHit> raycastAll(const glm::vec2&, const glm::vec2&, float, ObjectType type = ObjectType::Null);

    /**
	 * Performs a raycast that returns all hits along the ray from a world-space.
     *
     * @param direction Direction of the ray in world space (should be normalized).
     * @param maxDistance Maximum distance the ray can travel.
     * @param type Filter for object types to include in the raycast.
     * @return A vector of all RaycastHit results sorted by distance (if implemented that way).
     */
    std::vector<RaycastHit> raycastAll(const glm::vec2&, float, ObjectType type = ObjectType::Null);
};

#endif