#ifndef FE_PLAYER_STATS
#define FE_PLAYER_STATS

#include <glm/vec2.hpp>

struct PlayerMovementStats {
	float maxWalkSpeed = 2.5f;
	float groundAcceleration = 10.0f;
	float groundDeceleration = 10.0f;
	float airAcceleration = 8.0f;
	float airDeceleration = 8.0f;
	float jumpForce = 4.5f;
	float jumpCutMultiplier = 0.5f;
	float fallGravityMultiplier = 2.0f;
	bool enableWallSlide = true;
	float wallSlideSpeed = 1.5f;
	bool enableCoyoteTime = true;
	float coyoteTime = 0.15f;
	bool enableJumpBuffer = true;
	float jumpBufferTime = 0.15f;
	float dashSpeed = 8.0f;
	float dashDuration = 0.2f;
	float bounceForce = 6.0f;
	float maxFeatherFallingSpeed = 1.0f;
	float featherFallingDuration = 4.0f;
	float wallJumpForceY = 5.5f;
	float wallJumpForceX = 2.0f;
	float wallSnapSpeed = 4.0f;
	float wallSnapDistance = 5.0f;
	float wallSnapJump = 3.5f;
	float fallDamageSpeed = 7.9f;
};

struct PlayerRaycastConfig {
	float groundedRayLength;
	glm::vec2 groundedRayDir;
	float groundedOffsetX;
	float groundedOffsetY;

	float ceilingRayLength;
	glm::vec2 ceilingRayDir;
	float ceilingOffsetX;
	float ceilingOffsetY;

	float wallRayLength;
	glm::vec2 wallRayDir;
	float wallOffsetX;
	float wallOffsetY;
};

struct PlayerInputState {
	bool jumpPressed = false;
	bool jumpReleased = false;
	bool jumpHeld = false;
	float moveInput = 0.0f;
	bool lastJumpInput = false;
	float timeSinceLastRelease = 100.0f;
	glm::vec2 rightStick = { 0.0f, 0.0f };
};

struct PlayerCameraConfig {
	glm::vec2 deadZone = { 0.8f, 0.8f };
	float lookAheadDistance = 1.5f;
	float lookAheadSpeed = 4.0f;
	float lookAheadReturnSpeed = 10.0f;
	float defaultSmoothTime = 0.15f;
	float rightStickSmoothTime = 0.4f;
	float fastFallSmoothTime = 0.1f;
	float airSmoothTime = 0.2f;
	float smoothTimeTransitionSpeed = 2.0f;
	float airThresholdYSmooth = -10.0f;
	float fallCameraShakeThreshold = -15.0f;
	float verticalOffset = 0.2f;
	float rightStickDistance = 5.0f;
	float maxCameraSpeed = 10000.0f;
};

#endif