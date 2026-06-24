#include "include/Game/Objects/ShieldTankEnemy.hpp"
#include "include/AudioManager/AudioManager.hpp"

ShieldTankEnemy::ShieldTankEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;

	speed = 0.3f;
	damage = 0.0f;

	float colY = fromMap(float, "colliderPosY", data);
	float width = fromMap(float, "width", data);
	float height = fromMap(float, "height", data);

	float skinWidth = 0.001f;

	raycastOffsetX = (width / 2.0f) + skinWidth;
	raycastOffsetY = 0.0f;

	groundCheckDistance = std::abs(colY) + (height / 2.0f) + 0.001f;
	wallCheckDistance = std::abs(colY) + (height / 2.0f) - 0.01f;

	shieldRaycastOffsetX = raycastOffsetX + 0.02f;
	raycastGroundCheckOffsetX = raycastOffsetX + 0.2f;

	visiblityAngle = 1 / tan(radians(90.0f));
	visiblityDistance = 2.0f;

	shieldCooldown = 0.12f;
	attackDelay = 0.12f;

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

	auto wallHit = Raycast(
		glm::vec2(raycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		obstacleMask
	);

	if (groundHit.has_value() && !wallHit.has_value()) {
		SetVelocity(glm::vec2(direction * speed, GetVelocity().y));
		if (audio) {
			audio->PlayLooping("ui_1", 0.2f, 1.0f);
		}
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
		if (GetCurrentAnimation() != "BoilerShove") {
			Play("BoilerShove", 0.08f, false);
		}
		shieldOnCooldown = true;
		attackStarted = true;
		if (auto aum = Globals::GetGlobals().audioManager) {
			aum->PlaySound3D("player_spotted", GetTransform().GetTranslation(), 0.5f, 1.0f);
		}
	}
}

void ShieldTankEnemy::Physics(const float& deltaTime) {
	if (shieldOnCooldown) {
		shieldCooldownTimer += deltaTime;
		if (attackStarted && attackDelay < shieldCooldownTimer) {
			player->takeDamage(damage);
			player->SetVelocity(vec2(direction * 5.0f, player->GetVelocity().y));
		}
		if (shieldCooldownTimer >= shieldCooldown) {
			shieldOnCooldown = false;
			shieldCooldownTimer = 0.0f;

		}
	}
	if (GetVelocity().x == 0.0f && GetCurrentAnimation() != "BoilerIdle" && !(GetCurrentAnimation() == "BoilerShove" && IsPlaying())) {
		Play("BoilerIdle", 0.12f, true);
	}
	if (GetVelocity().x != 0.0f && GetCurrentAnimation() != "BoilerMove" && !(GetCurrentAnimation() == "BoilerShove" && IsPlaying())) {
		Play("BoilerMove", 0.12f, true);
	}
	Enemy::Physics(deltaTime);
}

void ShieldTankEnemy::ChangeState(shared_ptr<Player> player) {
	switch (state) {
	case EnemyState::Patrol: {
		if (audio) {
			audio->PlayLooping("boiler_engine", 0.08f, 1.0f, 7.5f, 0.8f);
		}
		if (seePlayer) {
			state = EnemyState::Chase;
			if (auto aum = Globals::GetGlobals().audioManager) {
				aum->PlaySound3D("player_spotted", GetTransform().GetTranslation(), 0.5f, 1.0f);
			}
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