#include "include/Game/Objects/ChargingEnemy.hpp"

ChargingEnemy::ChargingEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.8f;
	chargingSpeed = 3.0f;
	damage = 100.0f;
	raycastOffsetX = 0.326f;
	raycastOffsetY = 0.0f;
	groundCheckDistance = 0.61f;
	wallCheckDistance = 0.58f;

	raycastGroundCheckOffsetX = raycastOffsetX + 0.3f;

	chargeRaycastOffsetX = raycastOffsetX;
	stunDuration = 1.0f;
}

void ChargingEnemy::Physics(const float& deltaTime) {
	if (stunned) {
		SetVelocity(vec2(0, GetVelocity().y));
		realVelocity = vec2(0, realVelocity.y);
		stunTimer += deltaTime;
		if (stunTimer >= stunDuration) {
			stunned = false;
			stunTimer = 0.0f;

		}
		PhysicsNode::Physics(deltaTime);
	}
	else {
		Enemy::Physics(deltaTime);
	}
}

void ChargingEnemy::Chase(float dt) {

	auto wallHit = Raycast(
		glm::vec2(chargeRaycastOffsetX * direction, raycastOffsetY),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		obstacleMask
	);

	auto playerHit = Raycast(
		glm::vec2(chargeRaycastOffsetX * direction, raycastOffsetY),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		static_cast<uint32_t>(ObjectType::Player)
	);

	if (playerHit.has_value()) {
		player->takeDamage(damage);
	}
	if (wallHit.has_value()) {
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