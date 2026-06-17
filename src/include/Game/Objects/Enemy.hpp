#ifndef FE_ENEMY
#define FE_ENEMY

#include "include/Core/Object2D.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Core/Scene.hpp"
#include "include/AudioManager/AudioSource.hpp"
#include <memory>

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

	glm::vec2 lastPosition;
	glm::vec2 realVelocity;

	EnemyState state = EnemyState::Patrol;

	shared_ptr<Player> player;

	uint32_t obstacleMask = static_cast<uint32_t>(ObjectType::Wall) |
		static_cast<uint32_t>(ObjectType::Enemy) |
		static_cast<uint32_t>(ObjectType::Trap) |
		static_cast<uint32_t>(ObjectType::BreakableWall);

	unique_ptr<AudioSource> audio;

public:
	Enemy(const unordered_map<string, std::any>&);
	void Disable() noexcept override;

	void OnCollisionStay(Collider*) override;

	void OnCollisionExit(Collider*) override;

	void Physics(const float&) override;

	void UpdateState(float);

	virtual void Attack(shared_ptr<Player>);

	virtual void ChangeState(shared_ptr<Player>);

	virtual void Patrol(float);

	virtual void Chase(float) {};

	virtual void AttackState(float) {};

	virtual void DetectPlayer();

	void Init(shared_ptr<Scene>) override;

	bool AllRaycast(int);

	void ScaleToDirection();
};

#endif // FE_ENEMY
