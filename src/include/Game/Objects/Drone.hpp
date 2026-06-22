#pragma once
#include "include/Game/Objects/Enemy.hpp"
#include "include/Renderer/Light.hpp"
#include <memory>

class Drone : public Enemy {
private:
	int direction;
	float patrolSpeed;
	float patrolDistance;
	float diveSpeed;
	float visionRange;
	float visionAngle;
	float explosionRadius;
	float explosionDamage;
	float respawnHeight;
	float turnSpeed = 2.5f;
	float colorLightActivationTime;
	float colorLightActivationTimer;
	vec2 currentDiveVelocity = vec2(0.0f);
	bool isExploding = false;

	vec3 startPos;
	vec3 endPos;
	vec3 targetPos;
	vec3 diveTarget;
	vec3 colorDiffuseTarget;
	shared_ptr<Light> spotLight;

	void ReversePatrol();

public:
	Drone(const unordered_map<string, std::any>& data);

	void Init(shared_ptr<Scene> scene) override;
	void Physics(const float& deltaTime) override;
	void Process() override;


	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override {};

	void Spawn();
	void Respawn();

	void DetectPlayer() override;
	void ChangeState(shared_ptr<Player> player) override;
	void Chase(float dt) override;
	void Patrol(float dt) override;

	void Explode();
};