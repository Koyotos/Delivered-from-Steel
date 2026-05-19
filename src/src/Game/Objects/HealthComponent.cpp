#include "include/Game/Objects/HealthComponent.hpp"
#include "include/Globals/Globals.hpp"

void HealthComponent::Physics(float deltaTime) {
	if (!canTakeDamage) {
		damageTimer -= deltaTime;
		if (damageTimer <= 0.0f) {
			canTakeDamage = true;
		}
	}
}

void HealthComponent::TakeDamage(float damage) {
	if (!canTakeDamage || isDead) return;
	hp -= damage;
	canTakeDamage = false;
	damageTimer = damageCooldown;
	if (hp <= 0.0f) {
		Shatter();
	}
}

void HealthComponent::Shatter() {
	hp = 0.0f;
	if (isDead) return;
	isDead = true;
	respawnTimer = respawnDelay;
}

bool HealthComponent::CheckAndResetRespawn(float deltaTime) {
	if (isDead) {
		respawnTimer -= deltaTime;
		if (respawnTimer <= 0.0f) {
			isDead = false;
			hp = hpMax;
			return true;
		}
	}
	return false;
}