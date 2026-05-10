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

	float ledgeRayLength;
	float ledgeOffsetX;
	float ledgeLowerY;
	float ledgeUpperY;
};

struct PlayerInputState {
	bool jumpPressed = false;
	bool jumpReleased = false;
	bool jumpHeld = false;
	float moveInput = 0.0f;
	bool wantsToDrop = false;
	bool lastJumpInput = false;
	float timeSinceLastRelease = 100.0f;
	glm::vec2 rightStick = { 0.0f, 0.0f };
};

#endif