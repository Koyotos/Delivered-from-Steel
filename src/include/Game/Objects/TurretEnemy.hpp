#include "include/Game/Objects/Enemy.hpp"
#include "include/Game/Objects/Bullet.hpp"

#ifndef FE_TURRETENEMY
#define FE_TURRETENEMY

class TurretEnemy :
    public Enemy
{
private:
    std::shared_ptr<Object2D> barrel;
    std::shared_ptr<Object2D> barrelReal;
    std::shared_ptr<Bullet> bullet;

    bool bulletExist = false;
    
    bool isWaiting = true;
    float shotTime;
    float shotTimer = 0.0f;

    float barrelLockTime;
    float currentBarrelLock;

    float rotationSpeed;

    bool barrelLocked;

	bool playerInSight = false;

    float aimTolerance;
public:
    void Chase(float) override {};
    void Patrol(float) override;

    TurretEnemy(const unordered_map<string, std::any>&);

    void AttackState(float) override;

    void Update(float) override;

    void Attack(shared_ptr<Player> player) override {};

    void ChangeState(shared_ptr<Player> player) override;

    void RotateBarrel(float deltaTime);

	void Init(shared_ptr<Scene>) override;
};

#endif // FE_TURRETENEMY
