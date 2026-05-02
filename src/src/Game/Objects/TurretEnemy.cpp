#include "include/Game/Objects/TurretEnemy.hpp"

TurretEnemy::TurretEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.0f;
	damage = 40.0f;

	shotTime = 1.0f;

	visiblityAngle = 1 / tan(radians(90.0f));
	visiblityDistance = 2.0f;
}

void TurretEnemy::AttackState(float dt) {
	glm::vec2 dir = player->GetTransform().GetTranslation() - transform.GetTranslation();
	auto bulletHit = Raycast(
		glm::vec2(0.0f, 0.0f),
		dir,
		50.0f,
		ObjectType::Player
	);
	if (bulletHit.has_value()) {
		player->takeDamage(damage);
		isWaiting = true;
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
 