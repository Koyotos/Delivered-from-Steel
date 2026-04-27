#ifndef FE_PLAYER
#define FE_PLAYER

#include "include/Core/Object2D.hpp"
#include "include/IOManager/InputEvent.hpp"
#include "include/Renderer/Camera.hpp"

class Player : public Object2D {
private:
	shared_ptr<Camera> camera;

	float maxWalkSpeed = 2.5f;
	float groundAcceleration = 10.0f;
	float groundDeceleration = 10.0f;
	float airAcceleration = 8.0f;
	float airDeceleration = 8.0f;
	float jumpForce = 4.5f;
	float jumpCutMultiplier = 0.5f;
	float fallGravityMultiplier = 2.0f;
	bool canCutJump = false;

	bool enableWallSlide = true;
	float wallSlideSpeed = 1.5f;

	bool enableCoyoteTime = true;
	float coyoteTime = 0.15f;
	float coyoteTimeCounter = 0.0f;

	bool enableJumpBuffer = true;
	float jumpBufferTime = 0.15f;
	float jumpBufferCounter = 0.0f;

	bool isGrounded = false;
	bool isWalled = false;
	bool isWallSliding = false;
	float facingDirection = 1.0f;

	bool jumpPressed = false;
	bool jumpReleased = false;

	bool lastJumpInput = false;
	float timeSinceLastRelease = 100.0f;
	bool jumpHeld = false;

	float moveInput = 0.0f;

	float hpMax = 100.0f;
	float hp = hpMax;

	bool isHanging = false;
	float ledgeDropCooldown = 0.0f;
	bool wantsToDrop = false;
	float facingDirectionHang = 1.0f;

	glm::vec2 cameraTargetPos = { 0.0f, 0.0f };
	glm::vec2 cameraVelocity = { 0.0f, 0.0f };
	glm::vec2 deadZone = { 0.8f, 0.8f };
	float currentLookAheadX = 0.0f;
	float activeSmoothTime = 0.15f;
	float cameraShakeTimer = 0.0f;
	float cameraShakeIntensity = 0.0f;
	bool isCameraInitialized = false;
	bool isDead = false;
	float respawnDelay = 1.5f;
	float respawnTimer = 0.0f;

	float damageCooldown = 0.5f;
	float damageTimer = 0.0f;
	bool canTakeDamage = true;

	glm::vec2 platformVelocity = glm::vec2(0.0f);
	glm::vec3 respawnPoint;
	bool CheckGrounded();
	bool CheckWalled();
	bool CheckLeftWalled();
	bool CheckCeiling();

	float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
	glm::vec2 SmoothDamp(glm::vec2 current, glm::vec2 target, glm::vec2& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

	bool CheckLedge();
	float MoveTowards(float current, float target, float maxDelta);

public:

	void Update(float deltaTime) override;

	void SetCamera(shared_ptr<Camera>);
	void UpdateCamera(float deltaTime);
	void TriggerCameraShake(float duration, float intensity);

	Player();
	Player(const unordered_map<string, std::any>& data);
	void Process() override;
	bool Input(InputEvent& event) override;

	void takeDamage(float damage);

	void Shatter();

	void addPlatformVelocity(glm::vec2 velocity) { platformVelocity += velocity; }
};

#endif