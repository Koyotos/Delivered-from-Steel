#include "include/Game/Objects/ShieldTankEnemy.hpp"

ShieldTankEnemy::ShieldTankEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;

	speed = 0.3f;
	damage = 0.0f;

	raycastOffsetX = 0.211f;
	raycastOffsetY = 0.0f;

	groundCheckDistance = 0.581f;
	wallCheckDistance = 0.57f;

	shieldRaycastOffsetX = raycastOffsetX + 0.02f;

	shieldCooldown = 0.1f;

	raycastGroundCheckOffsetX = raycastOffsetX + 0.2f;

	chaseTime = 2.0f;
}

void ShieldTankEnemy::Chase(float dt) {
	glm::vec2 dir = player->GetTransform().GetTranslation() - transform.GetTranslation();
	if (!seePlayer) {
		if (chaseTimer == 0.0f) {
			if (dir.y > 0.0f) {
				playerJumpingOver = true;
			}
		}
		chaseTimer += dt;
		if (chaseTimer >= chaseTime) {
			state = EnemyState::Patrol;
			chaseTimer = 0.0f;

		}

		chaseTimer += dt;

		if (playerJumpingOver) {
			float dist = length(dir);
			if (dist > 2.0f) {
				playerJumpingOver = false;
			}
			else if(player->GetTransform().GetTranslation().y < transform.GetTranslation().y + 0.2f) {
				auto hit = Raycast(
					glm::vec2(0.0f),
					dir,
					dist,
					obstacleMask
				);
				if (hit.has_value()) {
					playerJumpingOver = false;
				}
				else if (direction != sign(dir.x)) {
					direction = sign(dir.x);

					ScaleToDirection();
				}
			}

		}
		else {
			dir = lastPlayerDir;
		}

	}
	else {
		chaseTimer = 0.0f;
	}
	lastPlayerDir = dir;

	dir.y = 0.0f;
	dir = glm::normalize(dir);

	auto groundHit = Raycast(
		glm::vec2(shieldRaycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		static_cast<uint32_t>(ObjectType::Wall)
	);

	auto enemyHit = Raycast(
		glm::vec2(raycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		static_cast<uint32_t>(ObjectType::Enemy)
	);

	if (groundHit.has_value() && !enemyHit.has_value()) {
		SetVelocity(glm::vec2(direction * speed, GetVelocity().y));
	}
	else {
 		SetVelocity(glm::vec2(0.0f, GetVelocity().y));
	}
}

void ShieldTankEnemy::AttackState(float dt) {
	Chase(dt);
	auto shieldHit = Raycast(
		glm::vec2(shieldRaycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		static_cast<uint32_t>(ObjectType::Player)
	);
	if (shieldHit.has_value() && !shieldOnCooldown) {
		shieldOnCooldown = true;
		player->takeDamage(damage);
		player->SetVelocity(vec2(direction * 5.0f, player->GetVelocity().y));
	}
}

void ShieldTankEnemy::Physics(float deltaTime) {
	if (shieldOnCooldown) {
		shieldCooldownTimer += deltaTime;
		if (shieldCooldownTimer >= shieldOnCooldown) {
			shieldOnCooldown = false;
			shieldCooldownTimer = 0.0f;

		}
	}
	Enemy::Physics(deltaTime);
}

void ShieldTankEnemy::ChangeState(shared_ptr<Player> player) {
	switch (state) {
	case EnemyState::Patrol: {
		if (seePlayer) {
			state = EnemyState::Chase;
		}
		break;
	}
	case EnemyState::Chase: {
		float enemyToPlayerDist = glm::length(player->GetTransform().GetTranslation() - transform.GetTranslation());
		if (enemyToPlayerDist < atakDystanse) {
			state = EnemyState::Attack;
		}
		break;
	}
	case EnemyState::Attack: {
		break;
	}
	}
}