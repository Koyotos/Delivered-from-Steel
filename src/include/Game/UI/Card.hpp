#ifndef FE_CARD
#define FE_CARD

#include "include/Game/UI/UIElement.hpp"

class Card : public UIElement {

	public:

	Card();
	Card(const std::unordered_map<std::string, std::any>& data);

	/*
	* This function should be called only inside CardSlot's Draw() function.
	*/
	void Draw() override;
};

#endif