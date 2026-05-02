#include "include/Game/Objects/ChargingEnemy.hpp"

ChargingEnemy::ChargingEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.8f;
	chargingSpeed = 3.0f;
	damage = 100.0f;
	raycastOffsetX = 0.5f;
	raycastOffsetY = 0.0f;
	groundCheckDistance = 0.461f;
	wallCheckDistance = 0.38f;

	chargeRaycastOffsetX = 0.11;
	stunDuration = 1.0f;
}

void ChargingEnemy::Update(float deltaTime) {
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

void ChargingEnemy::Chase(float dt) {

	auto wallHit = Raycast(
		glm::vec2(chargeRaycastOffsetX * direction, raycastOffsetY),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		ObjectType::Wall
	);

	auto enemyHit = Raycast(
		glm::vec2(chargeRaycastOffsetX * direction, raycastOffsetY),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		ObjectType::Enemy
	);

	auto trapHit = Raycast(
		glm::vec2(chargeRaycastOffsetX * direction, raycastOffsetY),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		ObjectType::Trap
	);

	if (enemyHit.has_value() || wallHit.has_value() || trapHit.has_value()) {
		stunned = true;
		state = EnemyState::Patrol;
		SetVelocity(glm::vec2(0.0f, GetVelocity().y));
	}
	else {
		SetVelocity(glm::vec2(chargingSpeed* direction, GetVelocity().y));
	}
}

void ChargingEnemy::ChangeState(shared_ptr<Player> player) {
	switch (state) {
	case EnemyState::Patrol: {
		if (seePlayer) {
			state = EnemyState::Chase;
		}
		break;
	}
	}
}