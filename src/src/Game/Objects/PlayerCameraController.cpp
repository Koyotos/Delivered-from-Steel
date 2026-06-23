#include "include/Game/Objects/PlayerCameraController.hpp"
#include <algorithm>
#include <cmath>

namespace {
	float MoveTowards(float current, float target, float maxDelta) {
		if (std::abs(target - current) <= maxDelta) {
			return target;
		}
		return current + std::copysign(maxDelta, target - current);
	}

	float SmoothDampValue(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
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

	glm::vec2 SmoothDampVec2(glm::vec2 current, glm::vec2 target, glm::vec2& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
		float vx = currentVelocity.x;
		float vy = currentVelocity.y;
		float rx = SmoothDampValue(current.x, target.x, vx, smoothTime, maxSpeed, deltaTime);
		float ry = SmoothDampValue(current.y, target.y, vy, smoothTime, maxSpeed, deltaTime);
		currentVelocity = glm::vec2(vx, vy);
		return glm::vec2(rx, ry);
	}
}

void PlayerCameraController::UpdateCamera(float deltaTime, const glm::vec2& playerPos, const glm::vec2& playerVel, float moveInput, const glm::vec2& rightStick, bool isGrounded) {
	if (!camera) return;

	if (!isCameraInitialized) {
		cameraTargetPos = playerPos;
		isCameraInitialized = true;
	}

	float targetSmoothTime = config.defaultSmoothTime;
	float targetLookAheadX = 0.0f;

	if (std::abs(playerVel.x) > 1.0f && std::abs(moveInput) > 0.01f) {
		targetLookAheadX = std::copysign(1.0f, playerVel.x) * config.lookAheadDistance;
	}

	if (isGrounded) {
		if (!platformYAssign) {
			float potencialLookAheadY = lastPlatformY - playerPos.y;
			if (abs(potencialLookAheadY) > 0.2f) {
				if (sign(targetLookAheadY) == sign(potencialLookAheadY) && std::abs(potencialLookAheadY) > std::abs(targetLookAheadY))
					targetLookAheadY = potencialLookAheadY;
				else if (sign(targetLookAheadY) != sign(potencialLookAheadY))
					targetLookAheadY = potencialLookAheadY;

				targetLookAheadY = std::clamp(targetLookAheadY, -1.0f, 0.5f);
				lastPlatformY = playerPos.y;
			}
			platformYAssign = true;
		}
	}
	else {
		platformYAssign = false;
	}

	currentLookAheadY = MoveTowards(targetLookAheadY, 0.0f, config.lookAheadReturnSpeed * deltaTime);

	float currentSpeed = (targetLookAheadX == 0.0f) ? config.lookAheadReturnSpeed : config.lookAheadSpeed;
	currentLookAheadX = MoveTowards(currentLookAheadX, targetLookAheadX, currentSpeed * deltaTime);

	glm::vec2 focusPosition = playerPos;
	focusPosition.x += currentLookAheadX;

	float xDistance = focusPosition.x - cameraTargetPos.x;
	float yDistance = focusPosition.y - cameraTargetPos.y;

	if (std::abs(xDistance) > config.deadZone.x)
		cameraTargetPos.x = focusPosition.x - std::copysign(config.deadZone.x, xDistance);

	if (std::abs(yDistance) > config.deadZone.y)
		cameraTargetPos.y = focusPosition.y - std::copysign(config.deadZone.y, yDistance);

	glm::vec2 desiredPosition = cameraTargetPos + glm::vec2(0.0f, config.verticalOffset - currentLookAheadY);

	if (glm::length(rightStick) > 0.1f) {
		desiredPosition += glm::vec2(rightStick.x, -rightStick.y) * config.rightStickDistance;
		targetSmoothTime = config.rightStickSmoothTime;
	}

	if (playerVel.y < config.fallCameraShakeThreshold) {
		TriggerCameraShake(0.1f, 0.1f);
	}

	if (std::abs(playerVel.y) > 0.1f) {
		if (playerVel.y < config.airThresholdYSmooth) {
			targetSmoothTime = config.fastFallSmoothTime;
		}
		else {
			targetSmoothTime = config.airSmoothTime;
		}
	}

	activeSmoothTime = MoveTowards(activeSmoothTime, targetSmoothTime, config.smoothTimeTransitionSpeed * deltaTime);

	glm::vec2 currentCamPos = camera->GetPos();
	glm::vec2 smoothedPosition = SmoothDampVec2(currentCamPos, desiredPosition, cameraVelocity, activeSmoothTime, config.maxCameraSpeed, deltaTime);

	if (cameraShakeTimer > 0.0f) {
		float randX = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * cameraShakeIntensity;
		float randY = ((rand() % 100) / 100.0f - 0.5f) * 2.0f * cameraShakeIntensity;
		smoothedPosition += glm::vec2(randX, randY);
		cameraShakeTimer -= deltaTime;
	}

	camera->SetPos(smoothedPosition);

	lastPlayerVelocity = playerVel;
}

void PlayerCameraController::TriggerCameraShake(float duration, float intensity) {
	cameraShakeTimer = duration;
	cameraShakeIntensity = intensity;
}