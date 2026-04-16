#include "include/Game/Objects/Player.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

Player::Player() : Object2D() {
	SetProcess(true);
	SetInput(true);
	SetDraw(true);
}

Player::Player(const unordered_map<string, std::any>& data) : Object2D(data) {
	objectType = ObjectType::Player;
}

void Player::SetCamera(shared_ptr<Camera> cam) {
	camera = cam;
}

float Player::MoveTowards(float current, float target, float maxDelta) {
	if (std::abs(target - current) <= maxDelta) {
		return target;
	}
	return current + std::copysign(maxDelta, target - current);
}

bool Player::CheckGrounded() {
	float rayLength = 0.01f;
	glm::vec2 rayDir(0.0f, -1.0f);
	float offsetX = 0.1f;
	float offsetY = -0.4f;

	auto hitCenter = raycast(glm::vec2(0.0f, offsetY), rayDir, rayLength, ObjectType::Wall);
	auto hitLeft = raycast(glm::vec2(-offsetX, offsetY), rayDir, rayLength, ObjectType::Wall);
	auto hitRight = raycast(glm::vec2(offsetX, offsetY), rayDir, rayLength, ObjectType::Wall);

	return hitCenter.has_value() || hitLeft.has_value() || hitRight.has_value();
}

bool Player::CheckWalled() {
	float rayLength = 0.01f;
	glm::vec2 rayDir(1.0f, 0.0f);
	float offsetX = 0.5f;
	float offsetY = 0.0f;
	auto hitRight = raycast(glm::vec2(offsetX, offsetY), rayDir, rayLength, ObjectType::Wall);
	auto hitLeft = raycast(glm::vec2(-offsetX, offsetY), -rayDir, rayLength, ObjectType::Wall);
	return hitRight.has_value() || hitLeft.has_value();
}

void Player::Process() {
	float deltaTime = Globals::GetGlobals().GetDeltaTime();

	isGrounded = CheckGrounded();
	isWalled = CheckWalled();

	float moveInput = 0.0f;
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_D)) {
		moveInput = 1.0f;
	}
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_A)) {
		moveInput = -1.0f;
	}

	float leftX = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_LEFT_X);
	if (std::abs(leftX) > 0.4f) {
		moveInput = (leftX > 0) ? 1.0f : -1.0f;
	}

	if (moveInput != 0.0f) {
		facingDirection = moveInput;
		Transform t = GetTransform();
		glm::vec3 scale = t.GetScale();
		scale.x = std::abs(scale.x) * facingDirection;
		t.SetScale(scale);
		SetTransform(t);
	}

	glm::vec2 currentVelocity = GetVelocity();

	if (isGrounded) {
		coyoteTimeCounter = coyoteTime;
	}
	else {
		coyoteTimeCounter -= deltaTime;
	}

	if (jumpPressed) {
		jumpBufferCounter = jumpBufferTime;
	}
	else {
		jumpBufferCounter -= deltaTime;
	}

	bool isSlidingDownWall = enableWallSlide && isWalled && !isGrounded && currentVelocity.y < 0.0f;
	isWallSliding = isSlidingDownWall && moveInput != 0.0f;

	bool canJump = (enableCoyoteTime && coyoteTimeCounter > 0.0f) || (!enableCoyoteTime && isGrounded);
	bool wantsToJump = (enableJumpBuffer && jumpBufferCounter > 0.0f) || (!enableJumpBuffer && jumpPressed);

	if (wantsToJump && canJump) {
		currentVelocity.y = jumpForce;
		jumpBufferCounter = 0.0f;
		coyoteTimeCounter = 0.0f;
		isGrounded = false;
	}
	else if (jumpReleased && currentVelocity.y > 0) {
		currentVelocity.y *= jumpCutMultiplier;
		coyoteTimeCounter = 0.0f;
	}

	float targetSpeed = moveInput * maxWalkSpeed;
	float acceleration = isGrounded ? (moveInput != 0.0f ? groundAcceleration : groundDeceleration) : (moveInput != 0.0f ? airAcceleration : airDeceleration);

	currentVelocity.x = MoveTowards(currentVelocity.x, targetSpeed, acceleration * deltaTime);

	if (currentVelocity.y < 0) {
		currentVelocity.y -= 10.0f * fallGravityMultiplier * deltaTime;
	}

	if (isWallSliding) {
		currentVelocity.y = std::max(currentVelocity.y, -wallSlideSpeed);
	}
	else {
		currentVelocity.y = std::max(currentVelocity.y, -maxFallSpeed);
	}

	SetVelocity(currentVelocity);

	jumpPressed = false;
	jumpReleased = false;
}

bool Player::Input(InputEvent& event) {
	if (!event.handled) {
		bool isJumpKey = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_SPACE) ||
			(event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_A);

		if (isJumpKey) {
			if (event.action == GLFW_PRESS) {
				jumpPressed = true;
				return true;
			}
			else if (event.action == GLFW_RELEASE) {
				jumpReleased = true;
				return true;
			}
		}
	}
	return false;
}

void Player::takeDamage(float damage) {
	hp -= damage;
	if (hp <= 0.0f) {
		Shatter();
	}
}

void Player::Shatter() {
	hp = 0.0f;
	Globals::GetGlobals().Log("Shatter");
	// tutaj mo¿na dodaæ logikê œmierci gracza, np. respawn, game over itp.
	hp = hpMax; // tymczasowo resetujemy HP, aby gracz móg³ kontynuowaæ grê
}