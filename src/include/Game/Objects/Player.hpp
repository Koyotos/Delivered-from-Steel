#ifndef FE_PLAYER
#define FE_PLAYER

#include "include/Core/Object2D.hpp"
#include "include/IOManager/InputEvent.hpp"
#include "include/Renderer/Camera.hpp"

class Player : public Object2D {
private:
	shared_ptr<Camera> camera;

	float maxWalkSpeed = 8.0f;
	float groundAcceleration = 40.0f;
	float groundDeceleration = 40.0f;
	float airAcceleration = 30.0f;
	float airDeceleration = 30.0f;
	float jumpForce = 8.0f;
	float jumpCutMultiplier = 0.5f;
	float fallGravityMultiplier = 2.0f;

	bool enableWallSlide = true;
	float wallSlideSpeed = 2.0f;

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

	float hpMax = 100.0f;
	float hp = hpMax;

	bool CheckGrounded();
	bool CheckWalled();
	float MoveTowards(float current, float target, float maxDelta);

public:

	void SetCamera(shared_ptr<Camera>);

	Player();
	Player(const unordered_map<string, std::any>&);
	void Process() override;
	bool Input(InputEvent& event) override;

	void takeDamage(float damage);

	void Shatter();
};

#endif