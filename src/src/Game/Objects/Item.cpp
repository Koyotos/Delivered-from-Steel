#include "include/Game/Objects/Item.hpp"

Item::Item(const unordered_map<string, std::any>& data) : Object3D(data) {
	objectType = ObjectType::Default;
	// Tutaj mo�na doda� logik� inicjalizacji przedmiotu, np. przypisanie karty
}

void Item::OnCollisionEnter(shared_ptr<Collider> other) {
	if (other->GetOwner()->GetObjectType() == ObjectType::Player) {
		shared_ptr<Player>  player = std::static_pointer_cast<Player>(other->GetOwner());
		GiveCard(player);
	}
}

void Item::GiveCard(shared_ptr<Player> player) {
	// Tutaj mo�na doda� logik� dawania karty graczowi
}
