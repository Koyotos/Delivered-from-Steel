#ifndef FE_PHYSICS_NODE
#define FE_PHYSICS_NODE

#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include "include/Core/VisualNode.hpp"

enum class ObjectType : uint32_t {
    Null = 0,
	Default = 1 << 0, // 1
    Wall = 1 << 1, // 2
    Enemy = 1 << 2, // 4
    Player = 1 << 3, // 8
    Trap = 1 << 4, // 16
    BreakableWall = 1 << 5,  // 32
	Bullet = 1 << 6 // 64
};

class PhysicsNode : public VisualNode, public enable_shared_from_this<PhysicsNode> {
    private:
	bool isStatic = false;
	bool isResolveCollision = true;

    vec2 velocity;
    shared_ptr<Collider> collider;
    shared_ptr<Shader> debugShader;

    protected:

    ObjectType objectType = ObjectType::Default;

    public:
	float maxFallSpeed = 50.0f;

    string Type() override;
    uint8_t RenderType() override;

    void SetCollider(shared_ptr<Collider> col);
    shared_ptr<Collider> GetCollider();

    void SetStatic(bool value);
    bool GetStatic() const;

    void Physics(float dt) override;

    void ResolveCollision(PhysicsNode& other);

    void ApplyForce(const vec2& force);

	void DrawDebug();
    void DrawBox();
    void DrawCapsule();

	void SetDebugShader(shared_ptr<Shader> shader);
    shared_ptr<Shader> GetDebugShader() const { return debugShader; };

    virtual void OnCollisionEnter(shared_ptr<Collider> other) {}
    virtual void OnCollisionStay(shared_ptr<Collider> other) {}
    virtual void OnCollisionExit(shared_ptr<Collider> other) {}

    vec2 GetVelocity() const { return velocity; }
    void SetVelocity(const vec2& v) { velocity = v; }

    void processCollisions();

    void Init();

	ObjectType GetObjectType() const { return objectType; }

    PhysicsNode();

    PhysicsNode(const unordered_map<string, std::any>&);

    /**
     * Performs a raycast from a world-space position with an explicit offset.
     *
     * @param offset Local-space offset added to the node's world position.
     * @param direction Direction of the ray in world space (should be normalized).
     * @param maxDistance Maximum distance the ray can travel.
     * @return RaycastHit if an object is hit, otherwise std::nullopt.
     */

    optional<RaycastHit> Raycast(const vec2&, const vec2&, float, uint32_t type = static_cast<uint32_t>(ObjectType::Null));

    /**
    * Performs a raycast from a world-space position.
    *
    * @param direction Direction of the ray in world space (should be normalized).
    * @param maxDistance Maximum distance the ray can travel.
    * @return RaycastHit if an object is hit, otherwise std::nullopt.
    */
    optional<RaycastHit> Raycast(const vec2&, float, uint32_t type = static_cast<uint32_t>(ObjectType::Null));

    /**
     * Performs a raycast that returns all hits along the ray from a world-space position with an explicit offset.
     *
     * @param offset Local-space offset added to the node's world position.
     * @param direction Direction of the ray in world space (should be normalized).
     * @param maxDistance Maximum distance the ray can travel.
     * @param type Filter for object types to include in the raycast.
     * @return A vector of all RaycastHit results sorted by distance (if implemented that way).
     */
    vector<RaycastHit> RaycastAll(const vec2&, const vec2&, float, uint32_t type = static_cast<uint32_t>(ObjectType::Null));

    /**
	 * Performs a raycast that returns all hits along the ray from a world-space.
     *
     * @param direction Direction of the ray in world space (should be normalized).
     * @param maxDistance Maximum distance the ray can travel.
     * @param type Filter for object types to include in the raycast.
     * @return A vector of all RaycastHit results sorted by distance (if implemented that way).
     */
    vector<RaycastHit> RaycastAll(const vec2&, float, uint32_t type = static_cast<uint32_t>(ObjectType::Null));
};

#endif