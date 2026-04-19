#ifndef FE_ENEMY
#define FE_ENEMY

#include "include/Core/Object2D.hpp"
#include "include/Game/Objects/Player.hpp"
class Enemy :
    public Object2D
{
private:
	float hpMax = 100.0f;
	float hp = hpMax;

	float damage = 100.0f;

public:
	Enemy(const unordered_map<string, std::any>&);

	void OnCollisionEnter(shared_ptr<Collider> other) override;

	void Attack(shared_ptr<Player> player);
};

#endif // FE_ENEMY
