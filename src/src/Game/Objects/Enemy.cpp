#include "include/Game/Objects/Enemy.hpp"
#include "include/Game/Objects/Player.hpp"

Enemy::Enemy(const unordered_map<string, std::any>& data) : Object2D(data) {
	objectType = ObjectType::Enemy;

	hpMax = 100.0f;
	hp = hpMax;

	speed = 0.6f;

	raycastOffsetX = 0.5f;
	raycastOffsetY = 0.0f;
	raycastGroundCheckOffsetX = 0.6f;

	groundCheckDistance = 0.461f;
	wallCheckDistance = 0.1f;

	visiblityAngle = 1/tan(radians(20.0f));
	visiblityDistance = 2.0f;

	damage = 100.0f;
	atakDystanse = 0.5f;
}

void Enemy::ChangeState(shared_ptr<Player> player) {
	switch (state) {
	case EnemyState::Patrol: {
		if (seePlayer) {
			state = EnemyState::Chase;
		}
		break;
	}
	case EnemyState::Chase: {
		if (!seePlayer) {
			state = EnemyState::Patrol;
		}
		else {
			float enemyToPlayerDist = glm::length(player->GetTransform().GetTranslation() - transform.GetTranslation());
			if (enemyToPlayerDist < atakDystanse) {
				state = EnemyState::Attack;
			}
		}
		break;
	}
	case EnemyState::Attack: {
		//  enemy will stay in attack state as long as player is in attack distance, otherwise it will switch to chase or patrol depending on if it sees the player
		break;
	}
	}
}

void Enemy::UpdateState(float dt) {
	DetectPlayer();
	ChangeState(player);
	switch (state) {
	case EnemyState::Patrol: {
		Patrol(dt);
		break;
	}
	case EnemyState::Chase: {
		Chase(dt);
		break;
	}
	case EnemyState::Attack: {
		AttackState(dt);
		break;
	}
	}
}

void Enemy::OnCollisionStay(shared_ptr<Collider> other) {
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (owner->GetObjectType() == ObjectType::Player) {
		shared_ptr<Player> player = static_pointer_cast<Player>(owner);
		Attack(player);
		glm::vec2 pos = GetTransform().GetTranslation();
		auto trans = player->GetTransform();
		trans.SetTranslation(trans.GetTranslation() + vec3(realVelocity, 0.0f));


		lastPosition = pos;
		player->SetTransform(trans);
	}
}

void Enemy::OnCollisionExit(shared_ptr<Collider> other) {
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (owner->GetObjectType() == ObjectType::Player) {

		player->addPlatformVelocity(realVelocity);
	}
}

void Enemy::Attack(shared_ptr<Player> player) {
	if (player) {
		player->takeDamage(damage);
	}
}

void Enemy::Init(shared_ptr<Scene> scene) {
	player = scene->GetPlayer();

	ScaleToDirection();
}

void Enemy::Physics(const float& deltaTime) {
	PhysicsNode::Physics(deltaTime);
	glm::vec2 pos = GetTransform().GetTranslation();
	realVelocity = pos - lastPosition;
	lastPosition = pos;
	UpdateState(deltaTime);
}

void Enemy::DetectPlayer() {
	if (player) {

		glm::vec3 enemyPos3 = transform.GetTranslation();
		enemyPos3.y -= groundCheckDistance/2;
		glm::vec3 playerPos3 = player->GetTransform().GetTranslation();

		glm::vec2 enemyPos(enemyPos3);
		glm::vec2 playerPos(playerPos3);

		glm::vec2 dir = playerPos - enemyPos;
		if (sign(dir.x) != sign(direction) || abs(dir.x/dir.y) < visiblityAngle) {
			seePlayer = false;
			return;
		}

		float dist = glm::length(dir);

		if (dist > 0.0001f && dist < visiblityDistance) {
			dir /= dist;

			auto hit = Raycast(
				glm::vec2(0.0f, -groundCheckDistance / 2),
				dir,
				dist,
				static_cast<uint32_t>(ObjectType::Wall)
			);

			seePlayer = !hit.has_value();
		}
		else {
			seePlayer = false;
		}
	}
}

bool Enemy::AllRaycast(int direction) {
	auto wallHit = Raycast(
		glm::vec2(raycastOffsetX * direction, raycastOffsetY),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		obstacleMask
	);

	auto groundHit = Raycast(
		glm::vec2(raycastGroundCheckOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		static_cast<uint32_t>(ObjectType::Wall)
	);

	return wallHit.has_value() || !groundHit.has_value();
}

void Enemy::Patrol(float dt) {
	if (AllRaycast(direction) && !AllRaycast(-direction)) {
		direction *= -1.0f;
		ScaleToDirection();
	}

	SetVelocity(glm::vec2(direction * speed, GetVelocity().y));
}

void Enemy::ScaleToDirection() {
	Transform t = GetTransform();
	glm::vec3 scale = t.GetScale();
	scale.x = std::abs(scale.x) * direction;
	t.SetScale(scale);
	SetTransform(t);
}