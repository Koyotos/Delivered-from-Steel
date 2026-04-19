#include "include/Game/Objects/Enemy.hpp"
#include "include/Game/Objects/Player.hpp"

Enemy::Enemy(const unordered_map<string, std::any>& data) : Object2D(data) {
	objectType = ObjectType::Enemy;
}

void Enemy::OnCollisionEnter(shared_ptr<Collider> other) {
	shared_ptr<PhysicsNode> owner = other->getOwner();
	if (owner->GetObjectType() == ObjectType::Player) {
		shared_ptr<Player> player = static_pointer_cast<Player>(owner);
		Attack(player);
	}
}

void Enemy::Attack(shared_ptr<Player> player) {
	if (player) {
		player->takeDamage(damage);
	}
}
