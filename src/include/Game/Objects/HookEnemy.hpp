#include "include/Game/Objects/Enemy.hpp"

#ifndef FE_HOOKENEMY
#define FE_HOOKENEMY

class HookEnemy :
    public Enemy
{
    public:
    void Chase(float) override;
    HookEnemy(const unordered_map<string, std::any>&);
    void AttackState(float) override;
	void Attack(shared_ptr<Player> player) override {};
};

#endif // FE_HOOKENEMY  

