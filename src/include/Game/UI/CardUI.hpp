#ifndef FE_CARD_UI
#define FE_CARD_UI

#include "include/Game/UI/UIElement.hpp"

/*
* @brief This class represents a card in the UI. It is a simple UIElement that can be placed inside a CardSlot.
*/
class CardUI : public UIElement {

	public:

	CardUI();
	CardUI(const std::unordered_map<std::string, std::any>& data);

	void Draw() override;
};

#endif