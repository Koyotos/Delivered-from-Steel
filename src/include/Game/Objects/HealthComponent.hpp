#ifndef FE_HEALTH_COMPONENT
#define FE_HEALTH_COMPONENT

class HealthComponent {
private:
	float hpMax = 100.0f;
	float hp = 100.0f;
	bool isDead = false;
	float respawnDelay = 1.5f;
	float respawnTimer = 0.0f;
	float damageCooldown = 0.5f;
	float damageTimer = 0.0f;
	bool canTakeDamage = true;

public:
	void Update(float deltaTime);
	void TakeDamage(float damage);
	void Shatter();
	bool CheckAndResetRespawn(float deltaTime);
	bool IsDead() const noexcept { return isDead; }
};

#endif