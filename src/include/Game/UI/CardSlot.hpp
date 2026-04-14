#ifndef FE_CARD_SLOT
#define FE_CARD_SLOT

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/UI/Card.hpp"
#include "include/Game/UI/Icon.hpp"

class CardSlot : public UIElement {
	
	private:

	std::shared_ptr<Card> card;
	std::shared_ptr<Icon> icon;

	public:

	CardSlot();
	CardSlot(const std::unordered_map<std::string, std::any>& data);

	void RemoveCard();
	void SetCard(std::shared_ptr<Card> newCard);
	void Draw() override;

};

#endif