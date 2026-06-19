#ifndef FE_PLAYER
#define FE_PLAYER

#include "include/Core/Object2D.hpp"
#include "include/IOManager/InputEvent.hpp"
#include "include/Game/Objects/PlayerMovementStats.hpp"
#include "include/Game/Objects/HealthComponent.hpp"
#include "include/Game/Objects/PlayerCameraController.hpp"
#include "include/Game/Objects/ParticleEmitterNode.hpp"
#include "include/Core/ISerializable.hpp"
#include "include/Game/Objects/OrbitalParticleSystem.hpp"
#include "include/AudioManager/AudioSource.hpp"
#include <memory>

class CardManager;

class Player final : public Object2D, public ISerializable {
private:
	PlayerMovementStats stats;
	PlayerInputState inputState;
	HealthComponent health;
	PlayerRaycastConfig raycastConfig;
	PlayerCameraController cameraController;
	std::shared_ptr<ParticleEmitterNode> deathEmitter;
	std::shared_ptr<ParticleEmitterNode> pixelEmitter;
	std::shared_ptr<OrbitalParticleSystem> pointVisualizer;
	std::shared_ptr<CardManager> cardManager;

	std::string respawnLevelName = "testLevel";

	bool isGrounded = false;
	bool isWalled = false;
	bool isWallSliding = false;
	bool isHanging = false;
	bool canCutJump = false;
	bool wasDead = false;
	bool isDashing = false;
	bool wasDashing = false;
	bool isBounceActive = false;
	bool isFeatherFalling = false;
	bool isWallSnaping = false;
	int wallJumpFacedDirection = 1.0f;
	bool isSuspended = false;

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

	float smoothedFallIntensity = 0.0f;

	glm::vec2 lastVelocity = glm::vec2(0.0f);
	glm::vec2 platformVelocity = glm::vec2(0.0f);
	glm::vec3 respawnPoint = glm::vec3(0.0f);

	void GatherInput(float deltaTime);
	bool HandleMovement(float deltaTime);
	void HandleAnimations();

	void UpdateVignette();

	uint32_t obstacleMask = static_cast<uint32_t>(ObjectType::Wall) |
		static_cast<uint32_t>(ObjectType::Enemy) |
		static_cast<uint32_t>(ObjectType::BreakableWall);

	unique_ptr<AudioSource> audio;

public:
	bool CheckGrounded();
	optional<RaycastHit> CheckRightWalledHit();
	optional<RaycastHit> CheckLeftWalledHit();
	bool CheckCeiling();

	Player();
	Player(const std::unordered_map<std::string, std::any>& data);
	void Disable() noexcept override;

	void Init(std::shared_ptr<Scene> scene) override;
	void Physics(const float& deltaTime) override;
	void Process() override;
	bool Input(InputEvent& event) override;
	void SetCardManager(std::shared_ptr<CardManager> mgr);

	void SetCamera(std::shared_ptr<Camera> cam);
	void TriggerCameraShake(float duration, float intensity);
	void takeDamage(float damage);
	void Shatter();
	bool IsHanging();
	void addPlatformVelocity(glm::vec2 velocity) { platformVelocity = velocity; }
	void SetRespawnPoint(glm::vec3 point, const std::string& levelName) {
		respawnPoint = point;
		respawnLevelName = levelName;
	}
	glm::vec3 GetRespawnPoint() const { return respawnPoint; }
	std::string GetRespawnLevelName() const { return respawnLevelName; }
	void SuspendForLoading() {
		isSuspended = true;
		SetVelocity(glm::vec2(0.0f));
	}
	void Unsuspend() {
		isSuspended = false;
	}
	bool IsWallSliding() const { return isWallSliding; }

	void ExecuteDash();
	void ExecuteBounce();
	void ExecuteFeatherFalling();
	void ExecuteDoubleJump();
	void ExecuteWallJump();
	void ExecuteWallSnap();
	bool CheckWallSnap();
	bool CheckWallJump();
	bool isDead() const { return health.IsDead(); }

	std::string GetSerializeKey() const override;
	nlohmann::json Serialize() const override;
	void Deserialize(const nlohmann::json& data) override;
};

#endif