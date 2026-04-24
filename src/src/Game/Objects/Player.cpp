#include "include/Game/Objects/Player.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

Player::Player() : Object2D() {
	SetProcess(true);
	SetInput(false);
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
	float offsetX = 0.1f;

	auto hitRightTop = raycast(glm::vec2(offsetX, 0.0f), rayDir, rayLength, ObjectType::Wall);
	auto hitLeftTop = raycast(glm::vec2(-offsetX, 0.0f), -rayDir, rayLength, ObjectType::Wall);

	auto hitRightMid = raycast(glm::vec2(offsetX, -0.2f), rayDir, rayLength, ObjectType::Wall);
	auto hitLeftMid = raycast(glm::vec2(-offsetX, -0.2f), -rayDir, rayLength, ObjectType::Wall);

	auto hitRightBot = raycast(glm::vec2(offsetX, -0.4f), rayDir, rayLength, ObjectType::Wall);
	auto hitLeftBot = raycast(glm::vec2(-offsetX, -0.4f), -rayDir, rayLength, ObjectType::Wall);

	return hitRightTop.has_value() || hitLeftTop.has_value() || hitRightMid.has_value() || hitLeftMid.has_value() || hitRightBot.has_value() || hitLeftBot.has_value();
}

bool Player::CheckLedge() {
	if (ledgeDropCooldown > 0.0f) return false;

	float rayLength = 0.05f;
	glm::vec2 rayDir(facingDirection, 0.0f);
	float offsetX = 0.1f;

	auto hitLower = raycast(glm::vec2((offsetX - 0.02f) * facingDirection, -0.1f), rayDir, rayLength, ObjectType::Wall);
	auto hitUpper = raycast(glm::vec2((offsetX - 0.02f) * facingDirection, 0.1f), rayDir, rayLength, ObjectType::Wall);

	return hitLower.has_value() && !hitUpper.has_value();
}

void Player::Process() {
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
		float normalizedSpeed = std::clamp((std::abs(leftX) - 0.1f) / (0.9f - 0.1f), 0.0f, 1.0f);
		moveInput = normalizedSpeed * std::copysign(1.0f, leftX);
	}

	if (!isHanging && std::abs(moveInput) > 0.01f) {
		facingDirection = std::copysign(1.0f, moveInput);
		Transform t = GetTransform();
		glm::vec3 scale = t.GetScale();
		scale.x = std::abs(scale.x) * facingDirection;
		t.SetScale(scale);
		SetTransform(t);
	}

	wantsToDrop = Globals::GetGlobals().GetKeyState(GLFW_KEY_S) || Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_LEFT_Y) > 0.5f;
}

void Player::Update(float deltaTime) {
	if (ledgeDropCooldown > 0.0f) ledgeDropCooldown -= deltaTime;

	glm::vec2 currentVelocity = GetVelocity();

	isGrounded = CheckGrounded();
	isWalled = CheckWalled();

	if (!isGrounded && currentVelocity.y < 0.0f && !isHanging) {
		if (CheckLedge()) {
			isHanging = true;
			currentVelocity = { 0, 0 };
			facingDirectionHang = facingDirection;
		}
	}

	if (isHanging) {
		if (jumpPressed) {
			isHanging = false;
			Transform t = GetTransform();
			t.SetTranslation(t.GetTranslation() + glm::vec3(0.0f, 0.1f, 0.0f));
			SetTransform(t);
			currentVelocity.y = jumpForce;
			currentVelocity.x = -facingDirection * 2.0f;
			canCutJump = true;
			jumpReleased = false;
			facingDirection = -facingDirection;
			ledgeDropCooldown = 0.3f;
			jumpPressed = false;
			jumpBufferCounter = 0.0f;
			coyoteTimeCounter = 0.0f;
		}
		else if (wantsToDrop) {
			isHanging = false;
			ledgeDropCooldown = 0.3f;
		}

		SetVelocity(currentVelocity);
		if (isHanging) {
			Transform t = GetTransform();
			glm::vec3 scale = t.GetScale();
			scale.x = std::abs(scale.x) * facingDirectionHang;
			t.SetScale(scale);
			SetTransform(t);
			UpdateCamera(deltaTime);
			return;
		}
	}

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
		canCutJump = true;
		jumpReleased = false;
		jumpBufferCounter = 0.0f;
		coyoteTimeCounter = 0.0f;
		isGrounded = false;
		if (!jumpHeld) {
			currentVelocity.y = jumpForce * jumpCutMultiplier;
		}
	}
	else if (jumpReleased && currentVelocity.y > 0 && canCutJump) {
		currentVelocity.y *= jumpCutMultiplier;
		coyoteTimeCounter = 0.0f;
		canCutJump = false;
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

	SetVelocity(currentVelocity);

	Transform t = this->GetTransform();
	t.SetTranslation(t.GetTranslation() + glm::vec3(currentVelocity * deltaTime, 0.0f));
	this->SetTransform(t);

	jumpPressed = false;
	jumpReleased = false;
	UpdateCamera(deltaTime);
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
	hp = hpMax;
}


float Player::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
	smoothTime = std::max(0.0001f, smoothTime);
	float omega = 2.0f / smoothTime;
	float x = omega * deltaTime;
	float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
	float change = current - target;
	float originalTo = target;
	float maxChange = maxSpeed * smoothTime;
	change = std::clamp(change, -maxChange, maxChange);
	target = current - change;
	float temp = (currentVelocity + omega * change) * deltaTime;
	currentVelocity = (currentVelocity - omega * temp) * exp;
	float output = target + (change + temp) * exp;
	if ((originalTo - current > 0.0f) == (output > originalTo)) {
		output = originalTo;
		currentVelocity = (output - originalTo) / deltaTime;
	}
	return output;
}

glm::vec2 Player::SmoothDamp(glm::vec2 current, glm::vec2 target, glm::vec2& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
	float vx = currentVelocity.x;
	float vy = currentVelocity.y;
	float rx = SmoothDamp(current.x, target.x, vx, smoothTime, maxSpeed, deltaTime);
	float ry = SmoothDamp(current.y, target.y, vy, smoothTime, maxSpeed, deltaTime);
	currentVelocity = glm::vec2(vx, vy);
	return glm::vec2(rx, ry);
}

void Player::UpdateCamera(float deltaTime) {
	if (!camera) return;

	glm::vec3 playerPos3D = GetTransform().GetTranslation();
	glm::vec2 playerPos = glm::vec2(playerPos3D.x, playerPos3D.y);
	glm::vec2 playerVel = GetVelocity();

	if (!isCameraInitialized) {
		cameraTargetPos = playerPos;
		isCameraInitialized = true;
	}

	float targetSmoothTime = 0.15f;
	float targetLookAheadX = 0.0f;

	if (std::abs(playerVel.x) > 1.0f && std::abs(moveInput) > 0.01f) {
		targetLookAheadX = std::copysign(1.0f, playerVel.x) * 1.5f;
	}

	float currentSpeed = (targetLookAheadX == 0.0f) ? 10.0f : 4.0f;
	currentLookAheadX = MoveTowards(currentLookAheadX, targetLookAheadX, currentSpeed * deltaTime);

	glm::vec2 focusPosition = playerPos;
	focusPosition.x += currentLookAheadX;

	float xDistance = focusPosition.x - cameraTargetPos.x;
	float yDistance = focusPosition.y - cameraTargetPos.y;

	if (std::abs(xDistance) > deadZone.x)
		cameraTargetPos.x = focusPosition.x - std::copysign(deadZone.x, xDistance);

	if (std::abs(yDistance) > deadZone.y)
		cameraTargetPos.y = focusPosition.y - std::copysign(deadZone.y, yDistance);

	glm::vec2 desiredPosition = cameraTargetPos + glm::vec2(0.0f, 0.2f);

	float rightX = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_RIGHT_X);
	float rightY = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_RIGHT_Y);
	glm::vec2 rightStick(rightX, -rightY);

	if (glm::length(rightStick) > 0.1f) {
		desiredPosition += rightStick * 5.0f;
		targetSmoothTime = 0.4f;
	}

	if (playerVel.y < -15.0f) {
		TriggerCameraShake(0.1f, 0.1f);
	}

	if (std::abs(playerVel.y) > 0.1f) {
		if (playerVel.y < -10.0f) {
			targetSmoothTime = 0.1f;
		}
		else {
			targetSmoothTime = 0.2f;
		}
	}

	activeSmoothTime = MoveTowards(activeSmoothTime, targetSmoothTime, 2.0f * deltaTime);

	glm::vec2 currentCamPos = camera->GetPos();
	glm::vec2 smoothedPosition = SmoothDamp(currentCamPos, desiredPosition, cameraVelocity, activeSmoothTime, 10000.0f, deltaTime);

	if (cameraShakeTimer > 0.0f) {
		float randX = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * cameraShakeIntensity;
		float randY = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * cameraShakeIntensity;
		smoothedPosition += glm::vec2(randX, randY);
		cameraShakeTimer -= deltaTime;
	}

	camera->SetPos(smoothedPosition);
}

void Player::TriggerCameraShake(float duration, float intensity) {
	cameraShakeTimer = duration;
	cameraShakeIntensity = intensity;
}
