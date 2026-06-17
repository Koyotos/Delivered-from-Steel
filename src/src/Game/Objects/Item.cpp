#include "include/Game/Objects/Item.hpp"

Item::Item(const unordered_map<string, std::any>& data) : Object3D(data) {
	objectType = ObjectType::Default;
	// Tutaj mo�na doda� logik� inicjalizacji przedmiotu, np. przypisanie karty
}

void Item::OnCollisionEnter(shared_ptr<Collider> other) {
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (!owner) return;
	if (owner->GetObjectType() == ObjectType::Player) {
		shared_ptr<Player>  player = std::static_pointer_cast<Player>(owner);
		GiveCard(player);
	}
}

void Item::GiveCard(shared_ptr<Player> player) {
	// Tutaj mo�na doda� logik� dawania karty graczowi
}
