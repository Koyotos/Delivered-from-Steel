#ifndef FE_CARD_UI
#define FE_CARD_UI

#include "include/Game/UI/UIElement.hpp"

class CardUI : public UIElement {

	public:

	CardUI();
	CardUI(const std::unordered_map<std::string, std::any>& data);

	/*
	* This function should be called only inside CardSlot's Draw() function.
	*/
	void Draw() override;
};

#endif