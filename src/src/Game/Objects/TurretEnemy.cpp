#include "include/Game/Objects/TurretEnemy.hpp"

TurretEnemy::TurretEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.0f;
	damage = 20.0f;
	raycastOffsetX = 0.5f;
	raycastOffsetY = 0.0f;
	groundCheckDistance = 0.461f;
	wallCheckDistance = 0.1f;
}

void TurretEnemy::Chase(float dt) {
	// TurretEnemy does not move, so no implementation needed here
}

void TurretEnemy::AttackState(float dt) {
	auto bulletHit = Raycast(
		glm::vec2(raycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		ObjectType::Player
	);
	if (bulletHit.has_value()) {
		player->takeDamage(damage);
	}
}
 