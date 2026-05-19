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

    float chargingSpeed;

    float chargeRaycastOffsetX;
public:
    void Chase(float) override;

    ChargingEnemy(const unordered_map<string, std::any>&);

    void Physics(float deltaTime) override;

    void ChangeState(shared_ptr<Player> player) override;

    void Attack(shared_ptr<Player> player) override {};
};

#endif // FE_CHARGINGENEMY

