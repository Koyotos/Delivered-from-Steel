#include "include/Game/Objects/Player.hpp"
#include "include/Game/Objects/BreakableWall.hpp"
#include "include/Globals/Globals.hpp"
#include "include/IOManager/IOManager.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/EngineController/EngineController.hpp"
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

	float colX = fromMap(float, "colliderPosX", data);
	float colY = fromMap(float, "colliderPosY", data);
	float radius = fromMap(float, "radius", data);
	float height = fromMap(float, "height", data);

	float skinWidth = 0.01f;
	float halfHeight = radius + height / 2;

	raycastConfig.groundedRayLength = radius * 2 - skinWidth;
	raycastConfig.groundedRayDir = { 1.0f, 0.0f };
	raycastConfig.groundedOffsetX = colX - (raycastConfig.groundedRayLength / 2.0f);
	raycastConfig.groundedOffsetY = -(-colY + halfHeight) - skinWidth;

	
	raycastConfig.ceilingRayLength = radius;
	raycastConfig.ceilingRayDir = { 1.0f, 0.0f };
	raycastConfig.ceilingOffsetX = colX - (raycastConfig.ceilingRayLength / 2.0f);
	raycastConfig.ceilingOffsetY = (colY + halfHeight) + skinWidth;

	raycastConfig.wallRayLength = (halfHeight * 2.0f) - (skinWidth * 2.0f);
	raycastConfig.wallRayDir = { 0.0f, -1.0f };
	raycastConfig.wallOffsetX = colX + radius + skinWidth;
	raycastConfig.wallOffsetY = (colY + halfHeight) - skinWidth;

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

	PlayerCameraConfig camConfig;
	camConfig.deadZone.x = GetSafe<float>(data, "deadZoneX", 0.8f);
	camConfig.deadZone.y = GetSafe<float>(data, "deadZoneY", 0.8f);
	camConfig.lookAheadDistance = GetSafe<float>(data, "lookAheadDistance", 1.5f);
	camConfig.lookAheadSpeed = GetSafe<float>(data, "lookAheadSpeed", 4.0f);
	camConfig.lookAheadReturnSpeed = GetSafe<float>(data, "lookAheadReturnSpeed", 10.0f);
	camConfig.defaultSmoothTime = GetSafe<float>(data, "defaultSmoothTime", 0.15f);
	camConfig.rightStickSmoothTime = GetSafe<float>(data, "rightStickSmoothTime", 0.4f);
	camConfig.fastFallSmoothTime = GetSafe<float>(data, "fastFallSmoothTime", 0.1f);
	camConfig.airSmoothTime = GetSafe<float>(data, "airSmoothTime", 0.2f);
	camConfig.smoothTimeTransitionSpeed = GetSafe<float>(data, "smoothTimeTransitionSpeed", 2.0f);
	camConfig.airThresholdYSmooth = GetSafe<float>(data, "airThresholdYSmooth", -10.0f);
	camConfig.fallCameraShakeThreshold = GetSafe<float>(data, "fallCameraShakeThreshold", -15.0f);
	camConfig.verticalOffset = GetSafe<float>(data, "verticalOffset", 0.2f);
	camConfig.rightStickDistance = GetSafe<float>(data, "rightStickDistance", 5.0f);
	camConfig.maxCameraSpeed = GetSafe<float>(data, "maxCameraSpeed", 10000.0f);

	cameraController.SetConfig(camConfig);
}

void Player::SetCamera(std::shared_ptr<Camera> cam) {
	cameraController.SetCamera(cam);
}

void Player::TriggerCameraShake(float duration, float intensity) {
	cameraController.TriggerCameraShake(duration, intensity);
}

void Player::SetCardManager(std::shared_ptr<CardManager> mgr) {
	cardManager = mgr;
}

void Player::Init(std::shared_ptr<Scene> scene) {
	for (auto& child : GetChildren()) {
		if (child->Type() == "ParticleEmitterNode") {
			auto emitter = std::dynamic_pointer_cast<ParticleEmitterNode>(child);

			if (child->GetName() == "DeathEmitter") {
				deathEmitter = emitter;
			}
			else if (child->GetName() == "PixelEmitter") {
				pixelEmitter = emitter;
			}
		}
		if (child->GetName() == "OrbitalPoints" && child->Type() == "OrbitalParticleSystem") {
			pointVisualizer = std::static_pointer_cast<OrbitalParticleSystem>(child);
		}
	}
}

bool Player::CheckGrounded() {
	float rayLength = 0.19f;
	glm::vec2 rayDir(1.0f, 0.0f);
	float offsetX = -0.095f;
	float offsetY = -0.41f;

	auto hitRight = Raycast(glm::vec2(raycastConfig.groundedOffsetX, raycastConfig.groundedOffsetY), raycastConfig.groundedRayDir, raycastConfig.groundedRayLength, obstacleMask);

	return hitRight.has_value();
}

bool Player::CheckCeiling() {
	auto hitCenter = Raycast(glm::vec2(raycastConfig.ceilingOffsetX, raycastConfig.ceilingOffsetY), raycastConfig.ceilingRayDir, raycastConfig.ceilingRayLength, obstacleMask);

	return hitCenter.has_value();
}

optional<RaycastHit> Player::CheckLeftWalledHit() {
	auto hitLeft = Raycast(glm::vec2(-raycastConfig.wallOffsetX, raycastConfig.wallOffsetY), raycastConfig.wallRayDir, raycastConfig.wallRayLength, obstacleMask);

	return hitLeft;
}

optional<RaycastHit> Player::CheckRightWalledHit() {
	auto hitRight = Raycast(glm::vec2(raycastConfig.wallOffsetX, raycastConfig.wallOffsetY), raycastConfig.wallRayDir, raycastConfig.wallRayLength, obstacleMask);

	return hitRight;
}

void Player::GatherInput(float deltaTime) {
	bool currentJumpRaw = Globals::GetGlobals().GetKeyState(GLFW_KEY_SPACE) || Globals::GetGlobals().GetGamepadBtnState(GLFW_GAMEPAD_BUTTON_A);

	if (currentJumpRaw && !inputState.lastJumpInput) {
		inputState.jumpPressed = true;
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
	if (isSuspended) return;
	Object2D::Process();
	float deltaTime = Globals::GetGlobals().GetDeltaTime();

	if (deathEmitter) {
		if (!wasDead && health.IsDead()) {
			deathEmitter->Burst(10);
		}
	}

	if (pixelEmitter) {
		if (!wasDead && health.IsDead()) {
			//pixelEmitter->isEmitting = true;
		}
		else if (!health.IsDead()) {
			pixelEmitter->isEmitting = false;
		}
	}
	if (health.IsDead()) {
		pointVisualizer->Reset();
	}
	else {
		int currentPoints = cardManager->getCurrentManaPoints();
		pointVisualizer->UpdatePlayerState(currentPoints);
	}

	wasDead = health.IsDead();

	if (health.IsDead()) {
		inputState.moveInput = 0.0f;
		if (ledgeDropCooldown > 0.0f) ledgeDropCooldown -= deltaTime;
		SetVelocity(glm::vec2(0.0f));

		if (health.CheckAndResetRespawn(deltaTime)) {
			SetVelocity(glm::vec2(0.0f));
			lastVelocity = glm::vec2(0.0f);
			Globals::GetGlobals().engineController->TriggerRespawn();
		}
		return;
	}

	health.Physics(deltaTime);
	GatherInput(deltaTime);
}

bool Player::HandleMovement(float deltaTime) {
	if (ledgeDropCooldown > 0.0f) ledgeDropCooldown -= deltaTime;

	glm::vec2 currentVelocity = GetVelocity();

	Transform t = this->GetTransform();

	cameraController.UpdateCamera(deltaTime, glm::vec2(t.GetTranslation().x, t.GetTranslation().y), GetVelocity(), inputState.moveInput, inputState.rightStick);

	isGrounded = CheckGrounded();
	bool isCeiling = CheckCeiling();
	optional<RaycastHit> WalledLeftHit = CheckLeftWalledHit();
	optional<RaycastHit> WalledRightHit = CheckRightWalledHit();
	bool isWalledLeft = WalledLeftHit.has_value();
	bool isWalledRight = WalledRightHit.has_value();
	isWalled = isWalledRight || isWalledLeft;

	if (isWalledRight && isWalledLeft && (WalledLeftHit.value().collider != WalledRightHit.value().collider)) {
		Shatter();
	}

	if (isGrounded) {
		coyoteTimeCounter = stats.coyoteTime;
		if (isCeiling) {
			Shatter();
		}
	}
	else {
		coyoteTimeCounter -= deltaTime;
	}

	if (isWallSnaping) {
		currentVelocity.y = 0.0f;
		currentVelocity.x = facingDirection * stats.dashSpeed;
		SetVelocity(currentVelocity);

		Transform t = this->GetTransform();
		t.SetTranslation(t.GetTranslation() + glm::vec3(currentVelocity * deltaTime, 0.0f));
		this->SetTransform(t);

		if (isWalled) {
			if (isWalledLeft && facingDirection == -1.0f || isWalledRight && facingDirection == 1.0f) {
				currentVelocity.x = beforeCardVelocityX;
				currentVelocity.y = stats.wallSnapJump;
				SetVelocity(currentVelocity);
				isWallSnaping = false;
				return false;
			}
		}
		if ((facingDirection == -1 && wallSnapPosX > GetTransform().GetTranslation().x) || (facingDirection == 1 && wallSnapPosX < GetTransform().GetTranslation().x)) {
			currentVelocity.x = beforeCardVelocityX;
			currentVelocity.y = stats.wallSnapJump;
			SetVelocity(currentVelocity);
			isWallSnaping = false;
		}

		return false;
	}

	if (isBounceActive) {
		float minBounceSpeed = 2.5f;
		if (isGrounded && lastSpeedForBounceY < -minBounceSpeed) {
			currentVelocity.y = std::max(stats.bounceForce, -lastSpeedForBounceY - stats.bounceForce / lastSpeedForBounceY);
			lastVelocity = currentVelocity;
			isBounceActive = false;
			isDashing = false;
		}

		lastSpeedForBounceY = currentVelocity.y;
		lastSpeedForBounceX = currentVelocity.x;
	}

	if (isDashing) {
		if (!wasDashing) {
			if (std::abs(inputState.moveInput) > 0.01f) {
				facingDirection = std::copysign(1.0f, inputState.moveInput);
			}
			beforeCardVelocityX = facingDirection * abs(beforeCardVelocityX);
		}
		wasDashing = true;
		dashTimer -= deltaTime;
		if (dashTimer <= 0.0f) {
			isDashing = false;
		}
		else {
			currentVelocity.y = 0.0f;
			currentVelocity.x = facingDirection * stats.dashSpeed;
			SetVelocity(currentVelocity);

			Transform t = this->GetTransform();
			t.SetTranslation(t.GetTranslation() + glm::vec3(currentVelocity * deltaTime, 0.0f));
			this->SetTransform(t);

			if (isWalled) {
				int dir = 0;
				if (isWalledLeft && facingDirection == -1.0f) {
					dir = -1;
				}
				else if (isWalledRight && facingDirection == 1.0f) {
					dir = 1;
				}

				if (dir != 0) {
					auto hitBreakableWall = Raycast(glm::vec2(dir * raycastConfig.wallOffsetX, raycastConfig.wallOffsetY), raycastConfig.wallRayDir, raycastConfig.wallRayLength, static_cast<uint32_t>(ObjectType::BreakableWall));
					if (hitBreakableWall.has_value()) {
						shared_ptr<BreakableWall> breakableWall = std::static_pointer_cast<BreakableWall>(hitBreakableWall->collider->GetOwner());
						if (breakableWall) {
							breakableWall->BreakWall();
						}
					}
					else {
						isDashing = false;
					}
				}
			}

		}
		return false;
	}
	else if (wasDashing) {
		currentVelocity.x = beforeCardVelocityX;
		beforeCardVelocityX = 0.0f;
		lastVelocity = currentVelocity;
		wasDashing = false;
	}

	if (isFeatherFalling) {
		featherFallingTimer -= deltaTime;
		if (featherFallingTimer <= 0.0f) {
			isFeatherFalling = false;
		}
		else {
			if (currentVelocity.y < -stats.maxFeatherFallingSpeed) {
				currentVelocity.y = lerp(currentVelocity.y, -stats.maxFeatherFallingSpeed, 0.5f);
			}
			lastVelocity = currentVelocity;
			SetVelocity(currentVelocity);
		}
	}

	if (inputState.jumpPressed) {
		jumpBufferCounter = stats.jumpBufferTime;
	}
	else {
		jumpBufferCounter -= deltaTime;
	}

	bool wantsToJump = (stats.enableJumpBuffer && jumpBufferCounter > 0.0f) || (!stats.enableJumpBuffer && inputState.jumpPressed);

	if (abs(lastVelocity.y) - stats.fallDamageSpeed > abs(currentVelocity.y)) {
		Shatter();
	}
	lastVelocity = currentVelocity;

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
		if (isFeatherFalling) {
			currentVelocity.y -= 10.0f * stats.fallGravityMultiplier * deltaTime /3;
		}
		else {
			currentVelocity.y -= 10.0f * stats.fallGravityMultiplier * deltaTime;
		}

	}
	else {
		currentVelocity.y -= 10.0f * deltaTime;
	}

	if (isWallSliding) {
		if (GetCurrentAnimation() != "CourierWallSlide") {
			Play("CourierWallSlide", 0.1f, true);
		}
		float target = -stats.wallSlideSpeed;
		if (currentVelocity.y < target)
		{
			currentVelocity.y += (10.0f * stats.fallGravityMultiplier + 25.0f) * deltaTime;

			currentVelocity.y = std::min(currentVelocity.y, target);
		}
	}
	else {
		currentVelocity.y = std::max(currentVelocity.y, -maxFallSpeed);
	}

	currentVelocity = currentVelocity + platformVelocity;
	platformVelocity = glm::vec2(0, 0);

	SetVelocity(currentVelocity);

	t.SetTranslation(t.GetTranslation() + glm::vec3(currentVelocity * deltaTime, 0.0f));
	this->SetTransform(t);

	inputState.jumpPressed = false;
	inputState.jumpReleased = false;

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
			auto hitGroundSoon = Raycast(glm::vec2(0.0f, -0.7f), glm::vec2(0.0f, -1.0f), anticipationDistance, static_cast<uint32_t>(ObjectType::Wall));
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

void Player::Physics(const float& deltaTime) {
	if (isSuspended) return;
	bool skipAnimations = HandleMovement(deltaTime);

	if (!skipAnimations) {
		HandleAnimations();
	}

	if (pointVisualizer) {
		glm::vec3 playerPos = GetTransform().GetTranslation();
		pointVisualizer->UpdateOrbit(deltaTime, playerPos);
	}

}

bool Player::Input(InputEvent& event) {
	if (isSuspended) return false;
	if (!event.handled) {
		//test save/load
		if (event.type == InputType::KEYBOARD && event.action == GLFW_PRESS) {
			if (event.key == GLFW_KEY_F5) {
				Globals::GetGlobals().wantsToSave = true;
				event.handled = true;
				return true;
			}
			if (event.key == GLFW_KEY_F9) {
				Globals::GetGlobals().wantsToLoad = true;
				event.handled = true;
				return true;
			}
		}
	}
	return false;
}

void Player::takeDamage(float damage) {
	health.TakeDamage(damage);
	if (health.GetHP() <= 0.0f) {
		Shatter();
	}
	else {
		if (Globals::GetGlobals().ioManager) {
			Globals::GetGlobals().ioManager->Vibrate(0.3f, 0.3f, 0.3f);
		}
	}
}

void Player::Shatter() {
	health.Shatter();
	isBounceActive = false;
	isDashing = false;
	isWallSnaping = false;
	isFeatherFalling = false;
	SetPhysics(false);
	SetDraw(false);
	Globals::GetGlobals().Log("Shatter");
	if (Globals::GetGlobals().ioManager) {
		Globals::GetGlobals().ioManager->Vibrate(0.7f, 0.7f, 0.3f);
	}
}

bool Player::IsHanging() {
	return isHanging;
}

void Player::ExecuteDash() {
	if (!isDashing)
		beforeCardVelocityX = GetVelocity().x;
	isDashing = true;
	isWallSnaping = false;
	dashTimer = stats.dashDuration;
	wasDashing = false;
}

void Player::ExecuteBounce() {
	isBounceActive = true;
	lastSpeedForBounceY = 0;
	lastSpeedForBounceX = 0;
}

void Player::ExecuteFeatherFalling() {
	isFeatherFalling = true;
	featherFallingTimer = stats.featherFallingDuration;
}

void Player::ExecuteDoubleJump() {
	isDashing = false;
	glm::vec2 vel = GetVelocity();
	vel.y = stats.jumpForce;
	SetVelocity(vel);
	lastVelocity = GetVelocity();
	canCutJump = false;
}

void Player::ExecuteWallJump() {
	isDashing = false;
	glm::vec2 vel = GetVelocity();

	float jumpDir = 0.0f;
	if (CheckLeftWalledHit().has_value()) jumpDir = 1.0f;
	else if (CheckRightWalledHit().has_value()) jumpDir = -1.0f;

	if (jumpDir != 0.0f) {
		vel.y = stats.wallJumpForceY;
		vel.x = jumpDir * stats.wallJumpForceX;
		facingDirection = jumpDir;
		SetVelocity(vel);
		lastVelocity = GetVelocity();

		coyoteTimeCounter = 0.0f;
		jumpBufferCounter = 0.0f;
	}
	canCutJump = false;
}

bool Player::CheckWallSnap() {
	glm::vec2 rayDir(facingDirection, 0.0f);

	auto hitDown = Raycast(glm::vec2(raycastConfig.wallOffsetX * facingDirection, raycastConfig.wallOffsetY - raycastConfig.wallRayLength), rayDir, stats.wallSnapDistance);
	auto hitUp = Raycast(glm::vec2(raycastConfig.wallOffsetX * facingDirection, raycastConfig.wallOffsetY), rayDir, stats.wallSnapDistance);
	float playerX = GetTransform().GetTranslation().x;

	if (hitDown.has_value()) {
		float hitX = hitDown->point.x;
		if (hitUp.has_value()) {
			wallSnapPosX = hitDown->distance < std::abs(hitUp->distance) ? hitX : hitUp->point.x;
		}
		else {
			wallSnapPosX = hitX;
		}
		return true;
	}
	else if (hitUp.has_value()) {
		wallSnapPosX = hitUp->point.x;
		return true;
	}
	return false;
}

void Player::ExecuteWallSnap() {
	beforeCardVelocityX = GetVelocity().x;

	lastVelocity = glm::vec2(0.0f);
	isWallSnaping = true;
	isDashing = false;
}

std::string Player::GetSerializeKey() const {
	return "player";
}

nlohmann::json Player::Serialize() const {
	nlohmann::json j;

	j["hp"] = this->health.GetHP();

	glm::vec3 pos = this->GetTransform().GetTranslation();
	j["posX"] = pos.x;
	j["posY"] = pos.y;
	j["posZ"] = pos.z;

	j["respawnPosX"] = this->respawnPoint.x;
	j["respawnPosY"] = this->respawnPoint.y;
	j["respawnPosZ"] = this->respawnPoint.z;
	j["respawnLevel"] = this->respawnLevelName;

	return j;
}

void Player::Deserialize(const nlohmann::json& data) {
	if (data.contains("hp")) {
		this->health.SetHP(data["hp"]);
	}
	if (data.contains("posX") && data.contains("posY") && data.contains("posZ")) {
		glm::vec3 loadedPos(data["posX"], data["posY"], data["posZ"]);
		Transform t = this->GetTransform();
		t.SetTranslation(loadedPos);
		this->SetTransform(t);
	}
	if (data.contains("respawnPosX") && data.contains("respawnPosY") && data.contains("respawnPosZ")) {
		this->respawnPoint = glm::vec3(data["respawnPosX"], data["respawnPosY"], data["respawnPosZ"]);
	}
	if (data.contains("respawnLevel")) {
		this->respawnLevelName = data["respawnLevel"];
	}
}