#include "include/Game/Objects/HookEnemy.hpp"

HookEnemy::HookEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.4f;
	damage = 20.0f;
	raycastOffsetX = 0.5f;
	raycastOffsetY = 0.0f;
	groundCheckDistance = 0.461f;
	wallCheckDistance = 0.1f;

	stunDuration = 0.5f;

	visiblityAngle = 1 / tan(radians(20.0f));
	visiblityDistance = 3.0f;
}

void HookEnemy::AttackState(float dt) {
	SetVelocity(vec2(0, GetVelocity().y));
	auto hookHit = Raycast(
		glm::vec2(raycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		ObjectType::Player
	);
	//if (hookHit.has_value()) {
	//	player->takeDamage(damage);
	//	player->SetVelocity(player->GetVelocity() + glm::vec2(0.0f, 5.0f));
	//}
}

void HookEnemy::ChangeState(shared_ptr<Player> player) {
	switch (state) {
	case EnemyState::Patrol: {
		if (seePlayer) {
			state = EnemyState::Attack;
		}
		break;
	}
	case EnemyState::Attack: {
		break;
	}
	}
}

void HookEnemy::Attack(shared_ptr<Player> player) {
	if (state == EnemyState::Attack) {
		player->takeDamage(damage);
		player->SetVelocity(glm::vec2(player->GetVelocity().x, 5.0f));
		stunned = true;
		EnemyState::Patrol;
	}
}

void HookEnemy::Update(float deltaTime) {
	if (stunned) {
		SetVelocity(vec2(0, GetVelocity().y));
		stunTimer += deltaTime;
		if (stunTimer >= stunDuration) {
			stunned = false;
			stunTimer = 0.0f;

		}
		PhysicsNode::Update(deltaTime);
	}
	else {
		Enemy::Update(deltaTime);
	}
}

