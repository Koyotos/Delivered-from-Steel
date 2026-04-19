#include "include/Game/Objects/Trap.hpp"
#include "include/Game/Objects/Player.hpp"

Trap::Trap(const unordered_map<string, std::any>& data) : Object3D(data) {
    objectType = ObjectType::Trap;
}


void Trap::OnCollisionStay(shared_ptr<Collider> other) {
    shared_ptr<PhysicsNode> owner = other->getOwner();
    if (owner->GetObjectType() == ObjectType::Player) {
        shared_ptr<Player> player = static_pointer_cast<Player>(owner);
        ActivateEffect(player);
    }
}

void Trap::ActivateEffect(shared_ptr<Player> player) {
    player->takeDamage(damage);
}