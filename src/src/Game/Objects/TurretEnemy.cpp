#include "include/Game/Objects/TurretEnemy.hpp"
#include <iostream>

TurretEnemy::TurretEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.0f;
	damage = 0.0f;

	shotTime = 2.0f;

	visiblityAngle = 1 / tan(radians(90.0f));
	visiblityDistance = 4.0f;

	barrelLockTime = 0.2f;

	rotationSpeed = 2.0f;

	barrelLocked = false;

	aimTolerance = 0.98f;

	groundCheckDistance = 0.0f;
}

void TurretEnemy::AttackState(float dt) {
	RotateBarrel(dt);
	if (!isWaiting) {
		if (!bullet->GetVisible() && playerInSight) {
			if (GetCurrentAnimation() != "TurretBaseAttack" || !IsPlaying()) {
				Play("TurretBaseAttack", 0.04f, false);
			}
			if (barrelReal->GetCurrentAnimation() != "TurretBarrelAttack" || !barrelReal->IsPlaying()) {
				barrelReal->Play("TurretBarrelAttack", 0.04f, false);
			}
			Transform bulletTransform = bullet->GetTransform();

			vec3 rotation = barrel->GetTransform().GetRotation();
			float angle = rotation.z;
			glm::vec2 dir = glm::vec2(
				cos(angle - radians(90.0f)),
				sign(GetTransform().GetScale().y) * sin(angle - radians(90.0f))
			);
			bullet->SetDirection(normalize(dir));

			bullet->SetVisible(true);
			bulletTransform.SetTranslation(barrel->GetTransform().GetTranslation() * GetTransform().GetScale().y + GetTransform().GetTranslation());
			bullet->SetTransform(bulletTransform);
			barrelLocked = true;
			currentBarrelLock = barrelLockTime;
			isWaiting = true;
			playerInSight = false;
		}
	}
}

void TurretEnemy::Patrol(float dt) {
	float dir = player->GetTransform().GetTranslation().x - transform.GetTranslation().x;
	direction = sign(dir);
}

void TurretEnemy::Physics(const float& dt) {
	if (isWaiting) {
		shotTimer += dt;
		if (shotTimer >= shotTime) {
			isWaiting = false;
			shotTimer = 0.0f;

		}
	}
	if (barrelLocked) {
		currentBarrelLock -= dt;

		if (currentBarrelLock <= 0.0f)
			barrelLocked = false;
	}
	if (GetCurrentAnimation() != "TurretBaseIdle" && !IsPlaying()) {
		Play("TurretBaseIdle", 0.04f, true);
	}
	if (barrel->GetCurrentAnimation() != "TurretBarrelIdle" && !barrel->IsPlaying()) {
		barrel->Play("TurretBarrelIdle", 0.04f, true);
	}
	Enemy::Physics(dt);
}

void TurretEnemy::ChangeState(shared_ptr<Player> player) {
	switch (state) {
	case EnemyState::Patrol: {
		if (seePlayer) {
			state = EnemyState::Attack;
		}
		break;
	}
	case EnemyState::Attack: {
		if (!seePlayer) {
			playerInSight = false;
			state = EnemyState::Patrol;
		}
		break;
	}
	}
}

void TurretEnemy::RotateBarrel(float deltaTime)
{
	if (barrelLocked)
		return;

	Transform barrelTransform = barrel->GetTransform();

	vec2 barrelPos = vec2(barrelTransform.GetTranslation() * GetTransform().GetScale().y + GetTransform().GetTranslation());

	vec2 playerPos = vec2(player->GetTransform().GetTranslation());

	float bulletSpeed =
		bullet->GetSpeed();

	vec2 playerVelocity =player->GetVelocity();
	playerVelocity.y = 0.0f;

	float travelTime = length(playerPos - barrelPos) / bulletSpeed;

	vec2 targetPos = playerPos + playerVelocity * travelTime;

	travelTime = length(targetPos - barrelPos) / bulletSpeed;

	targetPos =	playerPos + playerVelocity * travelTime;

	vec2 direction = normalize(targetPos - barrelPos);

	float targetAngle = sign(GetTransform().GetScale().y) * atan2(direction.y, direction.x) + radians(90.0f);

	float targetAngleClamp = targetAngle;

	if (targetAngleClamp > radians(179.0f))
		targetAngleClamp = radians(-89.0f);
	else if (targetAngleClamp > radians(89.0f))
		targetAngleClamp = radians(89.0f);
	else if (targetAngleClamp < radians(-89.0f))
		targetAngleClamp = radians(-89.0f);

	vec3 rotation = barrelTransform.GetRotation();

	float currentAngle = rotation.z;


	float angleDiff = atan2(
		sin(targetAngleClamp - currentAngle),
		cos(targetAngleClamp - currentAngle)
	);

	float maxStep = rotationSpeed * deltaTime;

	float step = glm::clamp(angleDiff, -maxStep, maxStep);

	float newAngle = currentAngle + step;

	if (abs(cos(targetAngle - newAngle)) > aimTolerance && abs(targetAngle - newAngle) < radians(90.0f)) {
		playerInSight = true;
	}
	else {
		playerInSight = false;
	}

	rotation.z = newAngle;

	barrelTransform.SetRotation(rotation);

	barrel->SetTransform(barrelTransform);

	barrelReal->SetTransform(barrelReal->GetTransform());
}

void TurretEnemy::Init(shared_ptr<Scene> scene) {
	Enemy::Init(scene);
	for(auto& child : GetChildren()) {
		if(child->Type() == "Bullet") {
			bullet = static_pointer_cast<Bullet>(child);
		}
		if(child->Type() == "Object2D") {
			barrel = static_pointer_cast<Object2D>(child);
		}
	}
	barrelReal = static_pointer_cast<Object2D>(barrel->GetChildren()[0]);
}

