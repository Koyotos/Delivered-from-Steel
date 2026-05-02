#include "include/Game/Objects/ShieldTankEnemy.hpp"

ShieldTankEnemy::ShieldTankEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;

	speed = 0.3f;
	damage = 0.0f;

	raycastOffsetX = 0.5f;
	raycastOffsetY = 0.0f;

	groundCheckDistance = 0.461f;
	wallCheckDistance = 0.1f;

	shieldRaycastOffsetX = 0.1f;
}

void ShieldTankEnemy::Chase(float dt) {
	glm::vec2 dir = player->GetTransform().GetTranslation() - transform.GetTranslation();
	dir.y = 0.0f;
	dir = glm::normalize(dir);

	auto groundHit = Raycast(
		glm::vec2(raycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		ObjectType::Wall
	);

	if (groundHit.has_value()) {
		SetVelocity(glm::vec2(dir.x * speed, GetVelocity().y));
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
		ObjectType::Player
	);
	if (!shieldHit.has_value()) {
		player->takeDamage(damage);
		player->SetVelocity( player->GetVelocity() + glm::vec2(direction * 5.0f, 0.0f));
	}
}