#include "include/Game/Objects/Enemy.hpp"
#include "include/Game/Objects/Player.hpp"

Enemy::Enemy(const unordered_map<string, std::any>& data) : Object2D(data) {
	objectType = ObjectType::Enemy;

	hpMax = 100.0f;
	hp = hpMax;

	speed = 0.6f;

	raycastOffsetX = 0.5f;
	raycastOffsetY = 0.0f;

	groundCheckDistance = 0.461f;
	wallCheckDistance = 0.1f;

	visiblityAngle = 2.0f;
	visiblityAngle = 1/tan(radians(60.0f));
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
		//  to latwiej bedzie ustawic w AttackState(dt);
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
	}
}

void Enemy::Attack(shared_ptr<Player> player) {
	if (player) {
		player->takeDamage(damage);
	}
}

void Enemy::Init(shared_ptr<Scene> scene) {
	player = scene->GetPlayer();
}

void Enemy::Update(float deltaTime) {
	PhysicsNode::Update(deltaTime);
	UpdateState(deltaTime);
}

void Enemy::DetectPlayer() {
	if (player) {

		glm::vec3 enemyPos3 = transform.GetTranslation();
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
				glm::vec2(0.0f),
				dir,
				dist,
				ObjectType::Wall
			);

			seePlayer = !hit.has_value();
		}
		else {
			seePlayer = false;
		}
	}
}

void Enemy::Patrol(float dt) {
	glm::vec2 dir(direction, 0.0f);

	auto wallHit = Raycast(
		glm::vec2(raycastOffsetX * direction, raycastOffsetY),
		dir,
		wallCheckDistance,
		ObjectType::Wall
	);

	auto enemyHit = Raycast(
		glm::vec2(raycastOffsetX * direction, raycastOffsetY),
		dir,
		wallCheckDistance,
		ObjectType::Enemy
	);

	auto trapHit = Raycast(
		glm::vec2(raycastOffsetX * direction, raycastOffsetY),
		dir,
		wallCheckDistance,
		ObjectType::Trap
	);

	auto groundHit = Raycast(
		glm::vec2(raycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		ObjectType::Wall
	);

	if (enemyHit.has_value() || wallHit.has_value() || trapHit.has_value() || !groundHit.has_value()) {
		direction *= -1.0f;

		Transform t = GetTransform();
		glm::vec3 scale = t.GetScale();
		scale.x = std::abs(scale.x) * direction;
		t.SetScale(scale);
		SetTransform(t);
	}

	SetVelocity(glm::vec2(direction * speed, GetVelocity().y));
}