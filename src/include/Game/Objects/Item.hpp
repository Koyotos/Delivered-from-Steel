#ifndef FE_ITEM
#define FE_ITEM

#include "include/Core/Object3D.hpp"
#include "include/Game/Objects/Card.hpp"
#include "include/Game/Objects/Player.hpp"

class Item :
    public Object3D
{
private:
	shared_ptr<Card> card;
public:
	Item(const unordered_map<string, std::any>&);

	void OnCollisionEnter(Collider* other) override;

	void GiveCard(shared_ptr<Player> player);
};

#endif // FE_ITEM
