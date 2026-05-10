#ifndef FE_PLAYER_CAMERA_CONTROLLER
#define FE_PLAYER_CAMERA_CONTROLLER

#include <glm/vec2.hpp>
#include <memory>
#include "include/Renderer/Camera.hpp"

class PlayerCameraController {
private:
	std::shared_ptr<Camera> camera;
	glm::vec2 cameraTargetPos = { 0.0f, 0.0f };
	glm::vec2 cameraVelocity = { 0.0f, 0.0f };
	glm::vec2 deadZone = { 0.8f, 0.8f };
	float currentLookAheadX = 0.0f;
	float activeSmoothTime = 0.15f;
	float cameraShakeTimer = 0.0f;
	float cameraShakeIntensity = 0.0f;
	bool isCameraInitialized = false;

public:
	void SetCamera(std::shared_ptr<Camera> cam);
	void UpdateCamera(float deltaTime, const glm::vec2& playerPos, const glm::vec2& playerVel, float moveInput, const glm::vec2& rightStick);
	void TriggerCameraShake(float duration, float intensity);
};

#endif