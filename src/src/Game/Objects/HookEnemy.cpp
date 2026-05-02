#include "include/Game/Objects/HookEnemy.hpp"

HookEnemy::HookEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.4f;
	damage = 30.0f;
	raycastOffsetX = 0.5f;
	raycastOffsetY = 0.0f;
	groundCheckDistance = 0.461f;
	wallCheckDistance = 0.1f;
}

void HookEnemy::Chase(float dt) {
	glm::vec2 dir = player->GetTransform().GetTranslation() - transform.GetTranslation();
	dir.y = 0.0f;
	dir = glm::normalize(dir);
	SetVelocity(glm::vec2(dir.x * speed, GetVelocity().y));
}

void HookEnemy::AttackState(float dt) {
	Chase(dt);
	auto hookHit = Raycast(
		glm::vec2(raycastOffsetX * direction, 0.0f),
		glm::vec2(0.0f, -1.0f),
		groundCheckDistance,
		ObjectType::Player
	);
	if (hookHit.has_value()) {
		player->takeDamage(damage);
		player->SetVelocity(player->GetVelocity() + glm::vec2(direction * 5.0f, 0.0f));
	}
}

