#include "include/Game/Objects/Enemy.hpp"

#ifndef FE_SHIELDTANKENEMY
#define FE_SHIELDTANKENEMY
class ShieldTankEnemy :
    public Enemy
{
private:
	float shieldRaycastOffsetX;

public:
    void Chase(float) override;

    ShieldTankEnemy(const unordered_map<string, std::any>&);

	void AttackState(float) override;

	void Attack(shared_ptr<Player> player) override {};
};

#endif // FE_SHIELDTANKENEMY
