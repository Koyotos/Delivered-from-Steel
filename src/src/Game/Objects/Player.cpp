#include "include/Game/Objects/Player.hpp"
#include "include/Globals/Globals.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

namespace {
	static float PlayerMoveTowards(float current, float target, float maxDelta) {
		if (std::abs(target - current) <= maxDelta) {
			return target;
		}
		return current + std::copysign(maxDelta, target - current);
	}

	static constexpr PlayerRaycastConfig raycastConfig{
		0.19f, {1.0f, 0.0f}, -0.095f, -0.41f,
		0.1f,  {1.0f, 0.0f}, -0.05f,  0.01f,
		0.38f, {0.0f, -1.0f}, 0.11f, -0.01f,
		0.05f, 0.1f, -0.1f, 0.1f
	};
}

template<typename T>
T GetSafe(const std::unordered_map<std::string, std::any>& data,
	const std::string& key,
	const T& defaultValue)
{
	auto it = data.find(key);
	if (it != data.end()) {
		try {
			return std::any_cast<T>(it->second);
		}
		catch (const std::bad_any_cast&) {
			return defaultValue;
		}
	}
	return defaultValue;
}

Player::Player() : Object2D() {
	SetProcess(true);
	SetInput(true);
	SetDraw(true);
}

Player::Player(const std::unordered_map<std::string, std::any>& data) : Object2D(data) {
	objectType = ObjectType::Player;
	Transform t = Transform(fromMap(std::vector<std::any>, "transform", data));
	respawnPoint = t.GetTranslation();

	stats.maxWalkSpeed = GetSafe<float>(data, "maxWalkSpeed", 2.5f);
	stats.groundAcceleration = GetSafe<float>(data, "groundAcceleration", 10.0f);
	stats.groundDeceleration = GetSafe<float>(data, "groundDeceleration", 10.0f);
	stats.airAcceleration = GetSafe<float>(data, "airAcceleration", 8.0f);
	stats.airDeceleration = GetSafe<float>(data, "airDeceleration", 8.0f);
	stats.jumpForce = GetSafe<float>(data, "jumpForce", 4.5f);
	stats.jumpCutMultiplier = GetSafe<float>(data, "jumpCutMultiplier", 0.5f);
	stats.fallGravityMultiplier = GetSafe<float>(data, "fallGravityMultiplier", 2.0f);
	stats.enableWallSlide = GetSafe<bool>(data, "enableWallSlide", true);
	stats.wallSlideSpeed = GetSafe<float>(data, "wallSlideSpeed", 1.5f);
	stats.enableCoyoteTime = GetSafe<bool>(data, "enableCoyoteTime", true);
	stats.coyoteTime = GetSafe<float>(data, "coyoteTime", 0.15f);
	stats.enableJumpBuffer = GetSafe<bool>(data, "enableJumpBuffer", true);
	stats.jumpBufferTime = GetSafe<float>(data, "jumpBufferTime", 0.15f);
}

void Player::SetCamera(std::shared_ptr<Camera> cam) {
	cameraController.SetCamera(cam);
}

void Player::TriggerCameraShake(float duration, float intensity) {
	cameraController.TriggerCameraShake(duration, intensity);
}

bool Player::CheckGrounded() {
	auto hitRight = Raycast(glm::vec2(raycastConfig.groundedOffsetX, raycastConfig.groundedOffsetY), raycastConfig.groundedRayDir, raycastConfig.groundedRayLength, ObjectType::Wall);
	return hitRight.has_value();
}

bool Player::CheckCeiling() {
	auto hitCenter = Raycast(glm::vec2(raycastConfig.ceilingOffsetX, raycastConfig.ceilingOffsetY), raycastConfig.ceilingRayDir, raycastConfig.ceilingRayLength, ObjectType::Wall);
	return hitCenter.has_value();
}

bool Player::CheckLeftWalled() {
	auto hitLeft = Raycast(glm::vec2(-raycastConfig.wallOffsetX, raycastConfig.wallOffsetY), raycastConfig.wallRayDir, raycastConfig.wallRayLength, ObjectType::Wall);
	return hitLeft.has_value();
}

bool Player::CheckRightWalled() {
	auto hitRight = Raycast(glm::vec2(raycastConfig.wallOffsetX, raycastConfig.wallOffsetY), raycastConfig.wallRayDir, raycastConfig.wallRayLength, ObjectType::Wall);
	return hitRight.has_value();
}

bool Player::CheckLedge() {
	if (ledgeDropCooldown > 0.0f) return false;
	glm::vec2 rayDir(facingDirection, 0.0f);
	auto hitLower = Raycast(glm::vec2((raycastConfig.ledgeOffsetX - 0.02f) * facingDirection, raycastConfig.ledgeLowerY), rayDir, raycastConfig.ledgeRayLength, ObjectType::Wall);
	auto hitUpper = Raycast(glm::vec2((raycastConfig.ledgeOffsetX - 0.02f) * facingDirection, raycastConfig.ledgeUpperY), rayDir, raycastConfig.ledgeRayLength, ObjectType::Wall);
	return hitLower.has_value() && !hitUpper.has_value();
}

void Player::GatherInput(float deltaTime) {
	bool currentJumpRaw = Globals::GetGlobals().GetKeyState(GLFW_KEY_SPACE) || Globals::GetGlobals().GetGamepadBtnState(GLFW_GAMEPAD_BUTTON_A);

	if (currentJumpRaw && !inputState.lastJumpInput) {
		if (inputState.timeSinceLastRelease > 0.08f) {
			inputState.jumpPressed = true;
		}
	}

	if (!currentJumpRaw && inputState.lastJumpInput) {
		inputState.jumpReleased = true;
		inputState.timeSinceLastRelease = 0.0f;
	}
	else {
		inputState.timeSinceLastRelease += deltaTime;
	}

	inputState.jumpHeld = currentJumpRaw;
	inputState.lastJumpInput = currentJumpRaw;
	inputState.moveInput = 0.0f;

	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_D)) {
		inputState.moveInput = 1.0f;
	}
	if (Globals::GetGlobals().GetKeyState(GLFW_KEY_A)) {
		inputState.moveInput = -1.0f;
	}

	float leftX = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_LEFT_X);
	if (std::abs(leftX) > 0.1f) {
		float normalizedSpeed = std::clamp((std::abs(leftX) - 0.1f) / (0.9f - 0.1f), 0.0f, 1.0f);
		inputState.moveInput = normalizedSpeed * std::copysign(1.0f, leftX);
	}

	if (!isHanging && std::abs(inputState.moveInput) > 0.01f) {
		facingDirection = std::copysign(1.0f, inputState.moveInput);
		Transform t = GetTransform();
		glm::vec3 scale = t.GetScale();
		scale.x = std::abs(scale.x) * facingDirection;
		t.SetScale(scale);
		SetTransform(t);
	}

	inputState.wantsToDrop = Globals::GetGlobals().GetKeyState(GLFW_KEY_S) || Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_LEFT_Y) > 0.5f;
	inputState.rightStick.x = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_RIGHT_X);
	inputState.rightStick.y = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_RIGHT_Y);
}

void Player::Process() {
	Object2D::Process();
	float deltaTime = Globals::GetGlobals().GetDeltaTime();

	if (health.IsDead()) {
		inputState.moveInput = 0.0f;
		if (ledgeDropCooldown > 0.0f) ledgeDropCooldown -= deltaTime;
		SetVelocity(glm::vec2(0.0f));

		if (health.CheckAndResetRespawn(deltaTime)) {
			Transform t = GetTransform();
			t.SetTranslation(respawnPoint);
			SetTransform(t);
		}
		return;
	}

	health.Update(deltaTime);
	GatherInput(deltaTime);
}

bool Player::HandleMovement(float deltaTime) {
	if (ledgeDropCooldown > 0.0f) ledgeDropCooldown -= deltaTime;

	glm::vec2 currentVelocity = GetVelocity();

	isGrounded = CheckGrounded();
	bool isWalledLeft = CheckLeftWalled();
	bool isWalledRight = CheckRightWalled();
	isWalled = isWalledRight || isWalledLeft;

	if (isWalledRight && isWalledLeft) {
		Shatter();
	}

	if (isGrounded) {
		coyoteTimeCounter = stats.coyoteTime;
		if (CheckCeiling()) {
			Shatter();
		}
	}
	else {
		coyoteTimeCounter -= deltaTime;
	}

	if (inputState.jumpPressed) {
		jumpBufferCounter = stats.jumpBufferTime;
	}
	else {
		jumpBufferCounter -= deltaTime;
	}

	bool wantsToJump = (stats.enableJumpBuffer && jumpBufferCounter > 0.0f) || (!stats.enableJumpBuffer && inputState.jumpPressed);

	if (!isGrounded && currentVelocity.y < 0.0f && !isHanging) {
		if (CheckLedge()) {
			isHanging = true;
			currentVelocity = { 0, 0 };
			facingDirectionHang = facingDirection;
		}
	}

	if (isHanging) {
		if (GetCurrentAnimation() != "CourierLedgeGrab") {
			Play("CourierLedgeGrab", 0.3f, true);
		}
		if (wantsToJump) {
			isHanging = false;
			Transform t = GetTransform();
			t.SetTranslation(t.GetTranslation() + glm::vec3(0.0f, 0.1f, 0.0f));
			SetTransform(t);
			currentVelocity.y = stats.jumpForce;
			currentVelocity.x = -facingDirection * 2.0f;
			canCutJump = true;
			inputState.jumpReleased = false;
			facingDirection = -facingDirection;
			ledgeDropCooldown = 0.3f;
			inputState.jumpPressed = false;
			jumpBufferCounter = 0.0f;
			coyoteTimeCounter = 0.0f;
		}
		else if (inputState.wantsToDrop) {
			isHanging = false;
			ledgeDropCooldown = 0.3f;
		}

		SetVelocity(currentVelocity);

		if (isHanging) {
			SetVelocity(glm::vec2(0.0f, 0.0f));
			Transform t = GetTransform();
			glm::vec3 scale = t.GetScale();
			scale.x = std::abs(scale.x) * facingDirectionHang;
			t.SetScale(scale);
			SetTransform(t);
			cameraController.UpdateCamera(deltaTime, glm::vec2(t.GetTranslation().x, t.GetTranslation().y), GetVelocity(), inputState.moveInput, inputState.rightStick);
			return true;
		}
	}

	bool isSlidingDownWall = stats.enableWallSlide && isWalled && !isGrounded && currentVelocity.y < 0.0f;
	isWallSliding = isSlidingDownWall && inputState.moveInput != 0.0f;

	bool canJump = (stats.enableCoyoteTime && coyoteTimeCounter > 0.0f) || (!stats.enableCoyoteTime && isGrounded);

	if (wantsToJump && canJump) {
		currentVelocity.y = stats.jumpForce;
		canCutJump = true;
		inputState.jumpReleased = false;
		jumpBufferCounter = 0.0f;
		coyoteTimeCounter = 0.0f;
		isGrounded = false;

		if (!inputState.jumpHeld) {
			currentVelocity.y = stats.jumpForce * stats.jumpCutMultiplier;
		}
	}
	else if (inputState.jumpReleased && currentVelocity.y > 0 && canCutJump) {
		currentVelocity.y *= stats.jumpCutMultiplier;
		coyoteTimeCounter = 0.0f;
		canCutJump = false;
	}

	float targetSpeed = inputState.moveInput * stats.maxWalkSpeed;
	float acceleration = isGrounded ?
		(inputState.moveInput != 0.0f ? stats.groundAcceleration : stats.groundDeceleration) : (inputState.moveInput != 0.0f ? stats.airAcceleration : stats.airDeceleration);

	currentVelocity.x = PlayerMoveTowards(currentVelocity.x, targetSpeed, acceleration * deltaTime);

	if (currentVelocity.y < 0) {
		currentVelocity.y -= 10.0f * stats.fallGravityMultiplier * deltaTime;
	}
	else {
		currentVelocity.y -= 10.0f * deltaTime;
	}

	if (isWallSliding) {
		if (GetCurrentAnimation() != "CourierWallSlide") {
			Play("CourierWallSlide", 0.1f, true);
		}
		currentVelocity.y = std::max(currentVelocity.y, -stats.wallSlideSpeed);
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

	inputState.jumpPressed = false;
	inputState.jumpReleased = false;

	cameraController.UpdateCamera(deltaTime, glm::vec2(t.GetTranslation().x, t.GetTranslation().y), GetVelocity(), inputState.moveInput, inputState.rightStick);

	return false;
}

void Player::HandleAnimations() {
	glm::vec2 currentVelocity = GetVelocity();
	bool isJumpPlaying = (GetCurrentAnimation() == "CourierJump" && IsPlaying());

	if (!isGrounded) {
		if (currentVelocity.y > 0.0f) {
			if (GetCurrentAnimation() != "CourierJump" && GetCurrentAnimation() != "CourierAirUp") {
				Play("CourierJump", 0.1f, false);
			}
			else if (!isJumpPlaying && GetCurrentAnimation() == "CourierJump") {
				Play("CourierAirUp", 0.1f, true);
			}
		}
		else {
			float anticipationDistance = 0.1f;
			auto hitGroundSoon = Raycast(glm::vec2(0.0f, -0.7f), glm::vec2(0.0f, -1.0f), anticipationDistance, ObjectType::Wall);
			if (hitGroundSoon.has_value()) {
				if (GetCurrentAnimation() != "CourierFall") {
					Play("CourierFall", 0.1f, false);
				}
			}
			else {
				if (!isJumpPlaying && GetCurrentAnimation() != "CourierFall") {
					Play("CourierAirLoop", 0.1f, true);
				}
			}
		}
	}
	else if (!(GetCurrentAnimation() == "CourierFall" && IsPlaying())) {
		bool isStopping = (GetCurrentAnimation() == "CourierWalk" && std::abs(currentVelocity.x) > 0.1f);
		if (inputState.moveInput != 0.0f || isStopping) {
			if (std::abs(currentVelocity.x) > 0.01f) {
				Play("CourierWalk", 0.1f, true);
			}
		}
		else {
			Play("CourierStanding", 0.2f, true);
		}
	}
}

void Player::Update(float deltaTime) {
	if (HandleMovement(deltaTime)) return;
	HandleAnimations();
}

bool Player::Input(InputEvent& event) {
	return false;
}

void Player::takeDamage(float damage) {
	health.TakeDamage(damage);
}

void Player::Shatter() {
	health.Shatter();
	Globals::GetGlobals().Log("Shatter");
}

bool Player::IsHanging() {
	return isHanging;
}