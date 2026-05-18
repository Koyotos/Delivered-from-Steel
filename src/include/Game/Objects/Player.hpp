#ifndef FE_PLAYER
#define FE_PLAYER

#include "include/Core/Object2D.hpp"
#include "include/IOManager/InputEvent.hpp"
#include "include/Game/Objects/PlayerMovementStats.hpp"
#include "include/Game/Objects/HealthComponent.hpp"
#include "include/Game/Objects/PlayerCameraController.hpp"
#include "include/Game/Objects/ParticleEmitterNode.hpp"

class Player final : public Object2D {
private:
	PlayerMovementStats stats;
	PlayerInputState inputState;
	HealthComponent health;
	PlayerCameraController cameraController;
	std::shared_ptr<ParticleEmitterNode> deathEmitter;
	std::shared_ptr<ParticleEmitterNode> pixelEmitter;

	bool isGrounded = false;
	bool isWalled = false;
	bool isWallSliding = false;
	bool isHanging = false;
	bool canCutJump = false;
	bool wasDead = false;
	bool isDashing = false;
	bool isBounceActive = false;
	bool isFeatherFalling = false;
	bool isWallSnaping = false;

	float lastSpeedForBounceY;
	float lastSpeedForBounceX;
	float dashTimer = 0.0f;
	float featherFallingTimer = 0.0f;
	float facingDirection = 1.0f;
	float facingDirectionHang = 1.0f;
	float coyoteTimeCounter = 0.0f;
	float jumpBufferCounter = 0.0f;
	float ledgeDropCooldown = 0.0f;
	float beforeCardVelocityX = 0.0f;
	float wallSnapPosX = 0.0f;

	glm::vec2 platformVelocity = glm::vec2(0.0f);
	glm::vec3 respawnPoint = glm::vec3(0.0f);

	void GatherInput(float deltaTime);
	bool HandleMovement(float deltaTime);
	void HandleAnimations();

	uint32_t obstacleMask = static_cast<uint32_t>(ObjectType::Wall) |
		static_cast<uint32_t>(ObjectType::Enemy) |
		static_cast<uint32_t>(ObjectType::BreakableWall);

public:
	bool CheckGrounded();
	bool CheckRightWalled();
	bool CheckLeftWalled();
	bool CheckCeiling();
	bool CheckLedge();

	Player();
	Player(const std::unordered_map<std::string, std::any>& data);

	void Init(std::shared_ptr<Scene> scene) override;
	void Update(float deltaTime) override;
	void Process() override;
	bool Input(InputEvent& event) override;

	void SetCamera(std::shared_ptr<Camera> cam);
	void TriggerCameraShake(float duration, float intensity);
	void takeDamage(float damage);
	void Shatter();
	bool IsHanging();
	void addPlatformVelocity(glm::vec2 velocity) { platformVelocity = velocity; }

	void ExecuteDash();
	void ExecuteBounce();
	void ExecuteFeatherFalling();
	void ExecuteDoubleJump();
	void ExecuteWallJump();
	void ExecuteWallSnap();
	bool CheckWallSnap();
};

#endif