#include "include/Game/Objects/Player.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

Player::Player() : Object2D() {
	SetProcess(true);
	SetInput(true);
	SetDraw(true);
}

Player::Player(const unordered_map<string, std::any>& data) : Object2D(data) {
	objectType = ObjectType::Player;
	Transform t = Transform(fromMap(vector<std::any>, "transform", data));
	respawnPoint = t.GetTranslation();
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
	if (isDead) {
		moveInput = 0.0f;
		return;
	}
	float deltaTime = Globals::GetGlobals().GetDeltaTime();

	bool currentJumpRaw = Globals::GetGlobals().GetKeyState(GLFW_KEY_SPACE) || Globals::GetGlobals().GetGamepadBtnState(GLFW_GAMEPAD_BUTTON_A);

	if (currentJumpRaw && !lastJumpInput) {
		if (timeSinceLastRelease > 0.08f) {
			jumpPressed = true;
		}
	}

	if (!currentJumpRaw && lastJumpInput) {
		jumpReleased = true;
		timeSinceLastRelease = 0.0f;
	}
	else {
		timeSinceLastRelease += deltaTime;
	}

	jumpHeld = currentJumpRaw;
	lastJumpInput = currentJumpRaw;


	moveInput = 0.0f;
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_D)) {
		moveInput = 1.0f;
	}
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_A)) {
		moveInput = -1.0f;
	}

	float leftX = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_LEFT_X);
	if (std::abs(leftX) > 0.1f) {
		float normalizedSpeed = std::clamp((std::abs(leftX) - 0.1f) / (1.0f - 0.1f), 0.0f, 1.0f);
		moveInput = normalizedSpeed * std::copysign(1.0f, leftX);
	}

	if (std::abs(moveInput) > 0.01f) {
		facingDirection = std::copysign(1.0f, moveInput);
		Transform t = GetTransform();
		glm::vec3 scale = t.GetScale();
		scale.x = std::abs(scale.x) * facingDirection;
		t.SetScale(scale);
		SetTransform(t);
	}
}

void Player::Update(float deltaTime) {
	if (isDead) {
		respawnTimer -= deltaTime;
		SetVelocity(glm::vec2(0.0f));
		if (respawnTimer <= 0.0f) {
			Transform t = GetTransform();
			t.SetTranslation(respawnPoint);
			SetTransform(t);

			hp = hpMax;
			isDead = false;

		}

		return;
	}
	if (!canTakeDamage) {
		damageTimer -= deltaTime;
		if (damageTimer <= 0.0f) {
			canTakeDamage = true;
		}
	}


	isGrounded = CheckGrounded();
	isWalled = CheckWalled();

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
		if (!jumpHeld) {
			currentVelocity.y = jumpForce * jumpCutMultiplier;
		}
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
	else {
		currentVelocity.y -= 10.0f * deltaTime;
	}

	if (isWallSliding) {
		currentVelocity.y = std::max(currentVelocity.y, -wallSlideSpeed);
	}
	else {
		currentVelocity.y = std::max(currentVelocity.y, -maxFallSpeed);
	}


	currentVelocity = currentVelocity + platformVelocity;
	platformVelocity = glm::vec2(0, 0);

	SetVelocity(currentVelocity);

	Transform t = this->GetTransform();
	t.SetTranslation(t.GetTranslation() + glm::vec3(currentVelocity * deltaTime, 0.0f));
	this->SetTransform(t);

	jumpPressed = false;
	jumpReleased = false;
}

bool Player::Input(InputEvent& event) {
	if (!event.handled) {
		//bool isJumpKey = (event.type == InputType::KEYBOARD && event.key == GLFW_KEY_SPACE) ||
		//	(event.type == InputType::GAMEPAD_BUTTON && event.key == GLFW_GAMEPAD_BUTTON_A);

		//if (isJumpKey) {
		//	if (event.action == GLFW_PRESS) {
		//		jumpPressed = true;
		//		return true;
		//	}
		//	else if (event.action == GLFW_RELEASE) {
		//		jumpReleased = true;
		//		return true;
		//	}
		//}
	}
	return false;
}

void Player::takeDamage(float damage) {
	if (!canTakeDamage || isDead) return;

	hp -= damage;

	canTakeDamage = false;
	damageTimer = damageCooldown;

	if (hp <= 0.0f) {
		Shatter();
	}
}

void Player::Shatter() {
	hp = 0.0f;
	isDead = true;
	respawnTimer = respawnDelay;
	Globals::GetGlobals().Log("Shatter");
	hp = hpMax;
}