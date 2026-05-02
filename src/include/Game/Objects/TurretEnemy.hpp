#include "include/Game/Objects/Enemy.hpp"

#ifndef FE_TURRETENEMY
#define FE_TURRETENEMY

class TurretEnemy :
    public Enemy
{
public:
    void Chase(float) override;

    TurretEnemy(const unordered_map<string, std::any>&);

    void AttackState(float) override;

    void Attack(shared_ptr<Player> player) override {};
};

#endif // FE_TURRETENEMY
