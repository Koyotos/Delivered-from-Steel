#ifndef FE_CARD_UI
#define FE_CARD_UI

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/CardType.hpp"

/*
* @brief This class represents a card in the UI. It is a simple UIElement that can be placed inside a CardSlot.
*/
class CardUI : public UIElement {

	private:

	CardType type;


	public:

	CardType GetCardType();

	CardUI();
	CardUI(const std::unordered_map<std::string, std::any>& data);

	void Draw(shared_ptr<Shader> sh = nullptr) override;
};

#endif