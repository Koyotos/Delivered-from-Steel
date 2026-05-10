#include "include/Game/Objects/TurretEnemy.hpp"

TurretEnemy::TurretEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.0f;
	damage = 0.0f;

	shotTime = 1.0f;

	visiblityAngle = 1 / tan(radians(90.0f));
	visiblityDistance = 2.0f;

	barrelLockTime = 0.5f;

	rotationSpeed = 5.0f;

	barrelLocked = false;

	aimTolerance = 0.98f;
}

void TurretEnemy::AttackState(float dt) {
	RotateBarrel(dt);
	if (!isWaiting) {
		if (!bullet->GetVisible() && playerInSight) {
			glm::vec2 dir = normalize(player->GetTransform().GetTranslation() - transform.GetTranslation());
			bullet->SetDirection(dir);
			bullet->SetVisible(true);
			Transform bulletTransform = bullet->GetTransform();
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

void TurretEnemy::Update(float dt) {
	if (isWaiting) {
		shotTimer += dt;
		if (shotTimer >= shotTime) {
			isWaiting = false;
			shotTimer = 0.0f;

		}
	}
	if (barrelLocked) {
		if (currentBarrelLock > 0.0f)
		{
			currentBarrelLock -= dt;

			if (currentBarrelLock <= 0.0f)
				barrelLocked = false;
		}
	}
	Enemy::Update(dt);
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

	vec2 direction = normalize(playerPos - barrelPos);

	float targetAngle = sign(GetTransform().GetScale().y) * atan2(direction.y, direction.x) + radians(90.0f);


	vec3 rotation = barrelTransform.GetRotation();

	float currentAngle = rotation.z;


	float angleDiff = atan2(
		sin(targetAngle - currentAngle),
		cos(targetAngle - currentAngle)
	);

	float newAngle =
		currentAngle +
		angleDiff * rotationSpeed * deltaTime;

	if (abs(cos(targetAngle - newAngle)) > aimTolerance) {
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
	if (static_pointer_cast<PhysicsNode>(GetChildren()[0])->GetObjectType() == ObjectType::Enemy) {
		barrel = static_pointer_cast<Object2D>(GetChildren()[1]);
		bullet = static_pointer_cast<Bullet>(GetChildren()[0]);
	}
	else {
		barrel = static_pointer_cast<Object2D>(GetChildren()[0]);
		bullet = static_pointer_cast<Bullet>(GetChildren()[1]);
	}
	barrelReal = static_pointer_cast<Object2D>(barrel->GetChildren()[0]);
}

