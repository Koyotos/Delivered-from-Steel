#include "include/Game/Objects/ChargingEnemy.hpp"
#include "include/Globals/Globals.hpp"
#include "include/AudioManager/AudioManager.hpp"

ChargingEnemy::ChargingEnemy(const unordered_map<string, std::any>& data) : Enemy(data)
{
	objectType = ObjectType::Enemy;
	speed = 0.8f;
	chargingSpeed = 3.0f;
	damage = 100.0f;


	float colY = fromMap(float, "colliderPosY", data);
	float width = fromMap(float, "width", data);
	float height = fromMap(float, "height", data);

	float skinWidth = 0.001f;

	raycastOffsetX = (width / 2.0f) + skinWidth;
	raycastOffsetY = 0.0f;

	groundCheckDistance = std::abs(colY) + (height / 2.0f) + 0.025f;
	wallCheckDistance = std::abs(colY) + (height / 2.0f) - 0.005f;

	raycastGroundCheckOffsetX = raycastOffsetX + 0.3f;

	chargeRaycastOffsetX = raycastOffsetX;
	stunDuration = 1.0f;
}

void ChargingEnemy::Physics(const float& deltaTime) {
	TriedToAttackPlayer = false;
	if (stunned) {
		if (GetCurrentAnimation() != "ChargerIdle") {
			Play("ChargerIdle", 0.12f, true);
		}
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
		if (GetCurrentAnimation() != "ChargerMove") {
			Play("ChargerMove", 0.12f, true);
		}
		Enemy::Physics(deltaTime);
	}
}

void ChargingEnemy::TryToAttackPlayer() {
	if (TriedToAttackPlayer) return;
	auto playerHit = Raycast(
		glm::vec2(chargeRaycastOffsetX * direction, raycastOffsetY),
		glm::vec2(0.0f, -1.0f),
		wallCheckDistance,
		static_cast<uint32_t>(ObjectType::Player)
	);

	if (playerHit.has_value()) {
		player->takeDamage(damage);
	}
	TriedToAttackPlayer = true;
}

void ChargingEnemy::Chase(float dt) {

	chargeCooldownTimer += dt;
	if (chargeCooldownTimer < chargeCooldown) {
			SetVelocity(glm::vec2(0.0f, GetVelocity().y));
			return;
	}

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
		if (auto aum = Globals::GetGlobals().audioManager) {
			aum->PlaySound3D("charger_wall", GetTransform().GetTranslation(), 0.4f, 1.0f, false, 14.0f, 1.0f);
		}
		audio->Stop();
	}
	else {
		SetVelocity(glm::vec2(chargingSpeed* direction, GetVelocity().y));
	}
}

void ChargingEnemy::ChangeState(shared_ptr<Player> player) {
	switch (state) {
	case EnemyState::Patrol: {
		audio->PlayLooping("charger_engine", 0.1f, 1.0f, 7.5f, 0.8f);
		if (seePlayer) {
			state = EnemyState::Chase;
			chargeCooldownTimer = 0;
			if (auto aum = Globals::GetGlobals().audioManager) {
				aum->PlaySound3D("player_spotted", GetTransform().GetTranslation(), 0.3f, 1.0f);
			}
			audio->PlayLooping("ui_1", 0.25f, 1.0f);
		}
		break;
	}
	}
}

void ChargingEnemy::OnCollisionStay(Collider* other) {
	Enemy::OnCollisionStay(other);
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (!owner) return;
	if (owner->GetObjectType() == ObjectType::Player) {
		TryToAttackPlayer();
	}
}