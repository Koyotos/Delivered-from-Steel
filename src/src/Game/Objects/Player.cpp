#include "include/Game/Objects/Player.hpp"
#include "include/Game/Objects/BreakableWall.hpp"
#include "include/Globals/Globals.hpp"
#include "include/IOManager/IOManager.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/EngineController/EngineController.hpp"
#include "include/AudioManager/AudioManager.hpp"
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
	audio = make_unique<AudioSource>(this);
	featherFallingAudio = make_unique<AudioSource>(this);
	footstepAudio = make_unique<AudioSource>(this);
}

void Player::Disable() noexcept {
	if (audio) {
		audio->Stop();
	}
	if (featherFallingAudio) {
		featherFallingAudio->Stop();
	}
	if (footstepAudio) {
		footstepAudio->Stop();
	}
	Object2D::Disable();
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
			else if (child->GetName() == "WallSnapEmitter") {
				wallSnapEmitter = emitter;
			}
		}
		else if (child->GetName() == "OrbitalPoints" && child->Type() == "OrbitalParticleSystem") {
			pointVisualizer = std::static_pointer_cast<OrbitalParticleSystem>(child);
		}
		else if (child->GetName() == "BounceBubble" && child->Type() == "Object2D") {
			bounceBubbleNode = std::static_pointer_cast<Object2D>(child);
			if (bounceBubbleNode) {
				bounceBubbleNode->Play("BubbleAnim", 0.15f, true);
			}
		}
		else if (child->GetName() == "outlineCollective" && child->Type() == "Object2D") {
			outlineCollectiveNode = std::static_pointer_cast<Object2D>(child);
		}
		else if (child->GetName() == "outlineYellow" && child->Type() == "Object2D") {
			outlineYellowNode = std::static_pointer_cast<Object2D>(child);
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
	if (std::abs(leftX) > Globals::GetGlobals().gamepadDeadzone) {
		inputState.moveInput = (leftX > 0.0f) ? 1.0f : -1.0f;
	}

	if (std::abs(inputState.moveInput) > 0.01f && !isDashing) {
		facingDirection = std::copysign(1.0f, inputState.moveInput);
		Transform t = GetTransform();
		glm::vec3 scale = t.GetScale();
		scale.x = std::abs(scale.x) * facingDirection;
		t.SetScale(scale);
		SetTransform(t);
	}

	inputState.rightStick.x = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_RIGHT_X);
	inputState.rightStick.y = Globals::GetGlobals().GetGamepadAxisState(GLFW_GAMEPAD_AXIS_RIGHT_Y);
}

void Player::Process() {
	if (isSuspended) return;
	Object2D::Process();
	float deltaTime = Globals::GetGlobals().GetDeltaTime();

	if (respawnProtectionTimer > 0.0f) {
		respawnProtectionTimer -= deltaTime;
	}

	if (health.IsDead()) {
		pointVisualizer->Reset();
	}
	else {
		int currentPoints = cardManager->getCurrentManaPoints();
		pointVisualizer->UpdatePlayerState(currentPoints, GetTransform().GetTranslation());
	}

	if (health.IsDead()) {
		inputState.moveInput = 0.0f;
		SetVelocity(glm::vec2(0.0f));

		if (health.CheckAndResetRespawn(deltaTime)) {
			SetVelocity(glm::vec2(0.0f));
			lastVelocity = glm::vec2(0.0f);
			respawnProtectionTimer = 0.2f;
			Globals::GetGlobals().engineController->TriggerRespawn();
		}
		return;
	}

	health.Physics(deltaTime);
	GatherInput(deltaTime);

	// przeniesc to pozniej do dedykowanej klasy
	UpdateVignette();
}

void Player::UpdateVignette() {
	float velY = GetVelocity().y;
	float speed = std::abs(std::min(velY, 0.0f));
	float threshold = stats.fallDamageSpeed * 0.3f;
	float target = std::clamp((speed - threshold) / (stats.fallDamageSpeed - threshold), 0.0f, 1.0f);

	float lerpSpeed = target > smoothedFallIntensity ? 6.0f : 2.5f;
	smoothedFallIntensity = PlayerMoveTowards(smoothedFallIntensity, target, lerpSpeed * Globals::GetGlobals().GetDeltaTime());

	float saturation = 1.0f - smoothedFallIntensity;
	Globals::GetGlobals().renderer->Reconfigure(RCMD_SATURATION_CONTROL, 1, 0.0f);
	Globals::GetGlobals().renderer->Reconfigure(RCMD_SATURATION_VALUE, 0, saturation);
}

bool Player::HandleMovement(float deltaTime) {
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

	if (auto aum = Globals::GetGlobals().audioManager) {
		if (isGrounded && !wasGrounded) {
			footstepDelayTimer = 0.10f;

			if (lastVelocity.y < -1.0f && lastVelocity.y > -stats.fallDamageSpeed) {
				float fallRatio = std::abs(lastVelocity.y) / stats.fallDamageSpeed;
				float volume = std::clamp(0.35f + (fallRatio * 0.7f), 0.35f, 1.05f);
				float pitch = std::clamp(1.1f - (fallRatio * 0.3f), 0.8f, 1.1f);
				aum->PlaySound2D("Player_Land", volume, pitch, false);
				footstepDelayTimer = 0.15f;
			}
		}
	}
	wasGrounded = isGrounded;

	if (isWalledRight && isWalledLeft && (WalledLeftHit.value().collider != WalledRightHit.value().collider)) {
		if (respawnProtectionTimer <= 0.0f) {
			Shatter();
			return false;
		}
	}

	if (isGrounded) {
		coyoteTimeCounter = stats.coyoteTime;
		if (isCeiling) {
			if (respawnProtectionTimer <= 0.0f) {
				Shatter();
				return false;
			}
		}
	}
	else {
		coyoteTimeCounter -= deltaTime;
	}

	if (isWallSnaping) {
		currentVelocity.y = 0.0f;
		currentVelocity.x = facingDirection * stats.dashSpeed;
		SetVelocity(currentVelocity);



		if (wallSnapEmitter && wallSnapEmitter->GetTargetSystem()) {
			wallSnapEmitter->GetTargetSystem()->KillParticlesBehindX(this->GetTransform().GetTranslation().x, facingDirection);
		}

		if (isWalled) {
			if (isWalledLeft && facingDirection == -1.0f || isWalledRight && facingDirection == 1.0f) {
				currentVelocity.x = 0.0f;
				currentVelocity.y = stats.wallSnapJump;
				SetVelocity(currentVelocity);
				isWallSnaping = false;
				return false;
			}
		}
		else if ((facingDirection == -1 && wallSnapPosX > GetTransform().GetTranslation().x) ||
			(facingDirection == 1 && wallSnapPosX < GetTransform().GetTranslation().x)) {
			currentVelocity.x = 0.0f;
			currentVelocity.y = stats.wallSnapJump;
			SetVelocity(currentVelocity);
			isWallSnaping = false;
		}
		else {
			Transform t = this->GetTransform();
			t.SetTranslation(t.GetTranslation() + glm::vec3(currentVelocity * deltaTime, 0.0f));
			this->SetTransform(t);
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
			if (auto aum = Globals::GetGlobals().audioManager) {
				aum->PlaySound2D("bounce", 0.16f, 1.2f, false);
			}
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
			if (featherFallingAudio) {
				featherFallingAudio->Stop();
			}
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
		if (respawnProtectionTimer <= 0.0f) {
			Shatter();
			return false;
		}
	}
	lastVelocity = currentVelocity;

	bool isSlidingDownWall = stats.enableWallSlide && isWalled && !isGrounded && currentVelocity.y < 0.0f;
	isWallSliding = isSlidingDownWall && inputState.moveInput != 0.0f;

	if (isDoubleJumping || isWallJumping) {
		if (currentVelocity.y <= 0.0f) {
			isDoubleJumping = false;
			isWallJumping = false;
		}
	}

	bool canJump = (stats.enableCoyoteTime && coyoteTimeCounter > 0.0f) || (!stats.enableCoyoteTime && isGrounded);

	if (wantsToJump && canJump) {
		currentVelocity.y = stats.jumpForce;
		canCutJump = true;
		inputState.jumpReleased = false;
		jumpBufferCounter = 0.0f;
		coyoteTimeCounter = 0.0f;
		isGrounded = false;

		if (auto aum = Globals::GetGlobals().audioManager) {
			aum->PlaySound2D("player_jump", 0.37f, 1.0f, false);
		}

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
			currentVelocity.y -= 10.0f * stats.fallGravityMultiplier * deltaTime / 3;
		}
		else {
			currentVelocity.y -= 10.0f * stats.fallGravityMultiplier * deltaTime;
		}

	}
	else {
		currentVelocity.y -= 10.0f * deltaTime;
	}

	if (isWallSliding) {
		float target = -stats.wallSlideSpeed;
		if (currentVelocity.y < target)
		{
			currentVelocity.y += (10.0f * stats.fallGravityMultiplier + 25.0f) * deltaTime;

			currentVelocity.y = std::min(currentVelocity.y, target);
		}

		wallSlideAudioTimer += deltaTime;

		if (wallSlideAudioTimer >= 0.15f) {
			audio->PlayLooping2D("wall_slide", 0.75f, 1.0f);
			wasWallSlinding = true;
		}
	}
	else {
		currentVelocity.y = std::max(currentVelocity.y, -maxFallSpeed);

		wallSlideAudioTimer = 0.0f;
		if (wasWallSlinding == true) {
			audio->Stop();
			wasWallSlinding = false;
		}
	}

	currentVelocity = currentVelocity + platformVelocity;
	platformVelocity = glm::vec2(0, 0);

	SetVelocity(currentVelocity);

	t.SetTranslation(t.GetTranslation() + glm::vec3(currentVelocity * deltaTime, 0.0f));
	this->SetTransform(t);

	inputState.jumpPressed = false;
	inputState.jumpReleased = false;

	if (footstepDelayTimer > 0.0f) {
		footstepDelayTimer -= deltaTime;
	}

	if (footstepAudio) {
		bool isWalking = isGrounded && footstepDelayTimer <= 0.0f && std::abs(currentVelocity.x) > 0.1f && !isDashing && !isWallSnaping;

		if (isWalking) {
			footstepAudio->PlayLooping2D("Player_Footsteps", 0.25f, 0.7f);
		}
		else {
			footstepAudio->Stop();
		}
	}

	return false;
}

void Player::HandleAnimations() {
	glm::vec2 currentVelocity = GetVelocity();
	string currentAnim = GetCurrentAnimation();
	bool isJumpPlaying = (currentAnim == "CourierJump" && IsPlaying());

	if (isDashing || isWallSnaping) {
		if (currentAnim != "CourierDash") {
			Play("CourierDash", 0.1f, true);
			if (outlineCollectiveNode) {
				outlineCollectiveNode->Play(isDashing ? "outlineRed" : "outlineBlue", 0.1f, true);
			}
		}
	}
	else if (isWallSliding) {
		if (currentAnim != "CourierWallSlide") {
			Play("CourierWallSlide", 0.1f, true);
			if (outlineYellowNode) {
				outlineYellowNode->Play("Grabing", 0.1f, true);
			}
		}
	}
	else if (!isGrounded) {
		if (currentVelocity.y > 0.0f) {
			if (currentAnim != "CourierJump" && currentAnim != "CourierAirUp") {
				Play("CourierJump", 0.1f, false);
				if (outlineCollectiveNode) {
					if (isWallJumping) {
						outlineCollectiveNode->Play("outlineOrange", 0.1f, false);
					}
					else if (isDoubleJumping) {
						outlineCollectiveNode->Play("outlineGreen", 0.1f, false);
					}
				}
				if (outlineYellowNode) {
					outlineYellowNode->Play("Jump", 0.1f, false);
				}
			}
			else if (!isJumpPlaying && currentAnim == "CourierJump") {
				Play("CourierAirUp", 0.1f, true);
				if (outlineCollectiveNode) {
					if (isWallJumping) {
						outlineCollectiveNode->Play("orangeAirUp", 0.1f, true);
					}
					else if (isDoubleJumping) {
						outlineCollectiveNode->Play("greenAirUp", 0.1f, true);
					}
				}
				if (outlineYellowNode) {
					outlineYellowNode->Play("AirUp", 0.1f, true);
				}
			}
		}
		else {
			float anticipationDistance = 0.1f;
			auto hitGroundSoon = Raycast(glm::vec2(0.0f, -0.7f), glm::vec2(0.0f, -1.0f), anticipationDistance, static_cast<uint32_t>(ObjectType::Wall));
			if (hitGroundSoon.has_value()) {
				if (currentAnim != "CourierFall") {
					Play("CourierFall", 0.1f, false);
					if (outlineYellowNode) {
						outlineYellowNode->Play("Fall", 0.1f, false);
					}
				}
			}
			else {
				if (!isJumpPlaying) {
					if (currentAnim != "CourierAirLoop") {
						Play("CourierAirLoop", 0.1f, true);
						if (outlineYellowNode) {
							outlineYellowNode->Play("AirLoop", 0.1f, true);
						}
					}
				}
			}
		}
	}
	else if (!(currentAnim == "CourierFall" && IsPlaying())) {
		bool isStopping = (currentAnim == "CourierWalk" && std::abs(currentVelocity.x) > 0.1f);
		if (inputState.moveInput != 0.0f || isStopping) {
			if (std::abs(currentVelocity.x) > 0.01f) {
				if (currentAnim != "CourierWalk") {
					Play("CourierWalk", 0.1f, true);
					if (outlineYellowNode) {
						outlineYellowNode->Play("Walking", 0.1f, true);
					}
				}
			}
		}
		else {
			if (currentAnim != "CourierStanding") {
				Play("CourierStanding", 0.2f, true);
				if (outlineYellowNode) {
					outlineYellowNode->Play("Standing", 0.2f, true);
				}
			}
		}
	}
}

void Player::Physics(const float& deltaTime) {
	if (isSuspended) return;

	if (!health.IsDead()) {
		bool skipAnimations = HandleMovement(deltaTime);
		if (!skipAnimations) {
			HandleAnimations();
		}
	}
	if (bounceBubbleNode) {
		Transform bubbleTransform;
		glm::vec3 localOffset = glm::vec3(0.0f, 0.0f, 0.01f);
		bubbleTransform.SetTranslation(localOffset);
		bubbleTransform.SetScale(glm::vec3(facingDirection, 1.0f, 1.0f));
		bounceBubbleNode->SetTransform(bubbleTransform);
		bounceBubbleNode->SetDraw(isBounceActive);
	}
	if (outlineCollectiveNode) {
		Transform outlineCollectiveTransform;
		glm::vec3 localOffset = glm::vec3(0.0f, 0.0f, -0.02f);
		outlineCollectiveTransform.SetTranslation(localOffset);
		outlineCollectiveNode->SetTransform(outlineCollectiveTransform);
		outlineCollectiveNode->SetDraw(isDashing || isDoubleJumping || isWallJumping || isWallSnaping);
	}
	if (outlineYellowNode) {
		Transform outlineYellowTransform;
		glm::vec3 localOffset = glm::vec3(0.0f, 0.0f, -0.01f);
		outlineYellowTransform.SetTranslation(localOffset);
		outlineYellowNode->SetTransform(outlineYellowTransform);
		outlineYellowNode->SetDraw(isFeatherFalling && !isDashing && !isDoubleJumping && !isWallJumping && !isWallSnaping);
	}

	if (pointVisualizer) {
		glm::vec3 playerPos = GetTransform().GetTranslation();
		pointVisualizer->UpdateOrbit(deltaTime, playerPos);
	}
	if (audio) {
		audio->Update();
	}
	if (featherFallingAudio) {
		featherFallingAudio->Update();
	}
	if (footstepAudio) {
		footstepAudio->Update();
	}
}

bool Player::Input(InputEvent& event) {
	if (isSuspended) return false;
	if (!event.handled) {
		if (event.type == InputType::KEYBOARD && event.action == GLFW_PRESS) {
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
	if (health.IsDead()) return;
	if (deathEmitter) {
		deathEmitter->Burst(10);
	}
	health.Shatter();
	isBounceActive = false;
	isDashing = false;
	isWallSnaping = false;
	isFeatherFalling = false;
	isWallJumping = false;
	isDoubleJumping = false;
	SetPhysics(false);
	SetDraw(false);
	if (bounceBubbleNode) bounceBubbleNode->SetDraw(false);
	if (outlineCollectiveNode) outlineCollectiveNode->SetDraw(false);
	if (outlineYellowNode) outlineYellowNode->SetDraw(false);
	if (wallSnapEmitter) {
		if (wallSnapEmitter->GetTargetSystem()) {
			wallSnapEmitter->GetTargetSystem()->Reset();
		}
	}
	if (audio) {
		audio->Stop();
	}
	if (featherFallingAudio) {
		featherFallingAudio->Stop();
	}
	if (footstepAudio) {
		footstepAudio->Stop();
	}
	Globals::GetGlobals().Log("Shatter");
	if (auto aum = Globals::GetGlobals().audioManager) {
		aum->PlaySound2D("player_death", 0.35f, 1.0f, false);
	}
	if (Globals::GetGlobals().ioManager) {
		Globals::GetGlobals().ioManager->Vibrate(0.7f, 0.7f, 0.3f);
	}
	Transform t = GetTransform();
	t.SetTranslation(respawnPoint);
	SetTransform(t);
}

void Player::ExecuteDash() {
	if (!isDashing)
		beforeCardVelocityX = GetVelocity().x;
	isDashing = true;
	isWallSnaping = false;
	dashTimer = stats.dashDuration;
	wasDashing = false;
	if (auto aum = Globals::GetGlobals().audioManager) {
		aum->PlaySound2D("player_dash", 0.2f, 1.0f, false);
	}
}

void Player::ExecuteBounce() {
	isBounceActive = true;
	lastSpeedForBounceY = 0;
	lastSpeedForBounceX = 0;
}

void Player::ExecuteFeatherFalling() {
	isFeatherFalling = true;
	featherFallingTimer = stats.featherFallingDuration;
	if (featherFallingAudio) {
		featherFallingAudio->PlayLooping2D("feather_falling", 0.16f, 1.0f);
	}
}

void Player::ExecuteDoubleJump() {
	isDashing = false;
	isWallSnaping = false;
	isWallJumping = false;
	isDoubleJumping = true;
	glm::vec2 vel = GetVelocity();
	vel.y = stats.jumpForce * 1.3f;
	SetVelocity(vel);
	lastVelocity = GetVelocity();
	canCutJump = false;
	if (auto aum = Globals::GetGlobals().audioManager) {
		aum->PlaySound2D("player_jump", 0.45f, 1.2f, false);
	}
	Play("CourierJump", 0.1f, false);
	if (outlineCollectiveNode) {
		outlineCollectiveNode->Play("outlineGreen", 0.1f, false);
	}
}

void Player::ExecuteWallJump() {
	isDashing = false;
	isWallSnaping = false;
	isWallJumping = true;
	isDoubleJumping = false;
	glm::vec2 vel = GetVelocity();

	vel.y = stats.wallJumpForceY;
	vel.x = wallJumpFacedDirection * stats.wallJumpForceX;
	SetVelocity(vel);
	lastVelocity = GetVelocity();

	coyoteTimeCounter = 0.0f;
	jumpBufferCounter = 0.0f;

	isWallSliding = false;
	platformVelocity = glm::vec2(0.0f);

	canCutJump = false;
	if (auto aum = Globals::GetGlobals().audioManager) {
		aum->PlaySound2D("player_jump", 0.5f, 0.8f, false);
	}
	Play("CourierJump", 0.1f, false);
	if (outlineCollectiveNode) {
		outlineCollectiveNode->Play("outlineOrange", 0.1f, false);
	}
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

	canCutJump = false;
	if (wallSnapEmitter) {
		glm::vec3 startPos = GetTransform().GetTranslation();
		glm::vec3 endPos = startPos;
		endPos.x = wallSnapPosX;
		float distance = std::abs(endPos.x - startPos.x);
		float particleDensity = 4.0f;
		int particleCount = static_cast<int>(distance * particleDensity);
		particleCount = std::clamp(particleCount, 3, 12);
		wallSnapEmitter->BurstAlongLine(startPos, endPos, particleCount, 0.1f);
	}
	if (auto aum = Globals::GetGlobals().audioManager) {
		aum->PlaySound2D("wall_snap", 0.16f, 1.2f, false);
	}
}

std::string Player::GetSerializeKey() const {
	return "player";
}

nlohmann::json Player::Serialize() const {
	nlohmann::json j;

	j["hp"] = this->health.GetHP();

	j["respawnPosX"] = this->respawnPoint.x;
	j["respawnPosY"] = this->respawnPoint.y;
	j["respawnPosZ"] = this->respawnPoint.z;
	j["respawnLevel"] = this->respawnLevelName;

	return j;
}

void Player::Deserialize(const nlohmann::json& data) {
	if (data.contains("hp")) {
		this->health.SetHP(data["hp"]);
		if (this->health.GetHP() > 0.0f) {
			this->health.Revive();
		}
	}
	if (data.contains("respawnPosX") && data.contains("respawnPosY") && data.contains("respawnPosZ")) {
		glm::vec3 loadedPos(data["respawnPosX"], data["respawnPosY"], data["respawnPosZ"]);
		Transform t = this->GetTransform();
		t.SetTranslation(loadedPos);
		this->SetTransform(t);
		this->respawnPoint = loadedPos;
	}
	if (data.contains("respawnLevel")) {
		this->respawnLevelName = data["respawnLevel"];
	}
}

bool Player::CheckWallJump() {
	float additionalOffsetXProcent = 1.5f;
	auto hitLeft = Raycast(glm::vec2(-(raycastConfig.wallOffsetX * additionalOffsetXProcent), raycastConfig.wallOffsetY), raycastConfig.wallRayDir, raycastConfig.wallRayLength, obstacleMask);
	auto hitRight = Raycast(glm::vec2((raycastConfig.wallOffsetX * additionalOffsetXProcent), raycastConfig.wallOffsetY), raycastConfig.wallRayDir, raycastConfig.wallRayLength, obstacleMask);

	bool isWalledLeft = hitLeft.has_value();
	bool isWalledRight = hitRight.has_value();
	wallJumpFacedDirection = isWalledLeft ? 1.0f : (isWalledRight ? -1.0f : 0.0f);
	return (isWalledLeft || isWalledRight);
}

void Player::StopAllLoopingAudio() {
	if (audio) audio->Stop();
	if (featherFallingAudio) featherFallingAudio->Stop();
	if (footstepAudio) footstepAudio->Stop();
}