#ifndef FE_CARD_SLOT
#define FE_CARD_SLOT

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/UI/CardUI.hpp"
#include "include/Game/UI/Icon.hpp"

/*
* @brief This class represents a slot for a card in the UI. It can hold one CardUI element and an Icon (keyboard or controller).
* It also handles the animation of adding and removing cards.
*/
class CardSlot : public UIElement {
	
	private:

	std::shared_ptr<CardUI> card;
	std::shared_ptr<CardUI> removedCard;
	std::shared_ptr<Icon> icon;

	vec2 origPos = vec2(0.0f, 0.0f);

	 public:

	CardSlot();
	CardSlot(const std::unordered_map<std::string, std::any>& data);

	vec2 iconOffset;

	string Type() override;

	/*
	* @brief Removes the current card from the slot.
	*/
	shared_ptr<CardUI> RemoveCard();

	void PlayUseAnimation();

	/*
	* @brief Sets a new card in the slot. If there is already a card, it will be removed first. 
	* The new card will be positioned at the slot's transform and will fade in.
	*/
	void SetCard(std::shared_ptr<CardUI> newCard);
	void SetIcon(std::shared_ptr<Icon> newIcon);

	void SetCardTint(vec3 color);

	void Draw(shared_ptr<Shader> sh = nullptr) override;

	void Process() override;

	void MoveTo(glm::vec2 target, float time, EaseType ease = EaseType::Linear, float delay = 0.0f);

	void ScaleCardTo(glm::vec2 target, float time, EaseType ease = EaseType::Linear, float delay = 0.0f);

	void FinishAllTweens();


	


};

#endif