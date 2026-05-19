#include "include/Game/Objects/Enemy.hpp"

#ifndef FE_SHIELDTANKENEMY
#define FE_SHIELDTANKENEMY
class ShieldTankEnemy :
    public Enemy
{
private:
	float shieldRaycastOffsetX;

	float shieldCooldown;
	bool shieldOnCooldown = false;
	float shieldCooldownTimer = 0.0f;

	bool playerJumpingOver = false;

	float chaseTime;
	float chaseTimer = 0.0f;
	vec2 lastPlayerDir;
public:
    void Chase(float) override;

    ShieldTankEnemy(const unordered_map<string, std::any>&);

	void AttackState(float) override;

	void Attack(shared_ptr<Player> player) override {};

	void ChangeState(shared_ptr<Player> player) override;

	void Physics(const float&) override;
};

#endif // FE_SHIELDTANKENEMY
