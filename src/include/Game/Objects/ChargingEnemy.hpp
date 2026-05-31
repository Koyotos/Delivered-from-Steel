#include "include/Game/Objects/Enemy.hpp"

#ifndef FE_CHARGINGENEMY
#define FE_CHARGINGENEMY
class ChargingEnemy :
    public Enemy
{
private:  
	bool stunned = false;
	float stunDuration;
	float stunTimer = 0.0f;

	float chargeCooldown = 0.5f;
	float chargeCooldownTimer = 0.0f;

    float chargingSpeed;

    float chargeRaycastOffsetX;

	bool TriedToAttackPlayer = false;

	void TryToAttackPlayer();
public:
    void Chase(float) override;

    ChargingEnemy(const unordered_map<string, std::any>&);

    void Physics(const float& deltaTime) override;

    void ChangeState(shared_ptr<Player> player) override;

    void Attack(shared_ptr<Player> player) override {};

	void OnCollisionStay(shared_ptr<Collider> other) override;
};

#endif // FE_CHARGINGENEMY

