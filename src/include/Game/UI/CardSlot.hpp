#ifndef FE_CARD_SLOT
#define FE_CARD_SLOT

#include "include/Game/UI/UIElement.hpp"

class Card;
class Icon;

class CardSlot : public UIElement {
	
	private:

	std::shared_ptr<Card> card;
	std::shared_ptr<Icon> icon;

	public:

	void removeCard();
	void ChangeCard(std::shared_ptr<Card> newCard);
	void Draw() override;

};

#endif