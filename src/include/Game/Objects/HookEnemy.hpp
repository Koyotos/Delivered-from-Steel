#include "include/Game/Objects/Enemy.hpp"

#ifndef FE_HOOKENEMY
#define FE_HOOKENEMY

class HookEnemy :
    public Enemy
{
private:
    bool stunned = false;
    float stunDuration;
    float stunTimer = 0.0f;
public:
    void Update(float) override;
    HookEnemy(const unordered_map<string, std::any>&);
    void AttackState(float) override;
    void Attack(shared_ptr<Player> player) override;

    void ChangeState(shared_ptr<Player> player) override;
};

#endif // FE_HOOKENEMY  

