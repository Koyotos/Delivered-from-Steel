#ifndef FE_ENEMY
#define FE_ENEMY

#include "include/Core/Object2D.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Core/Scene.hpp"

enum class EnemyState {
	Patrol,
	Chase,
	Attack
};

class Enemy :
	public Object2D
{
protected:
	float hpMax;
	float hp = hpMax;

	float speed;

	float raycastOffsetX;
	float raycastGroundCheckOffsetX;
	float raycastOffsetY;

	float groundCheckDistance;
	float wallCheckDistance;

	float visiblityAngle;
	float visiblityDistance;

	float damage;
	float atakDystanse;

	bool seePlayer = false;
	int direction = 1;
	EnemyState state = EnemyState::Patrol;

	shared_ptr<Player> player;

public:
	Enemy(const unordered_map<string, std::any>&);

	void OnCollisionStay(shared_ptr<Collider>) override;

	void Update(float) override;

	void UpdateState(float);

	virtual void Attack(shared_ptr<Player>);

	virtual void ChangeState(shared_ptr<Player>);

	virtual void Patrol(float);

	virtual void Chase(float) {};

	virtual void AttackState(float) {};

	virtual void DetectPlayer();

	void Init(shared_ptr<Scene>) override;

	bool AllRaycast(int);
};

#endif // FE_ENEMY
