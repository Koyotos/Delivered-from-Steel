#ifndef CARD_MANAGER
#define CARD_MANAGER

#include "include/Core/Node.hpp"
#include "include/Game/Objects/Card.hpp"
#include "include/Game/UI/CardSlot.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Core/ISerializable.hpp"
#include "include/Game/UI/Counter.hpp"



/*
 * @brief component class for handling all card-related operations.
 * It is created inside the EngineController's Init function.
 * It also creates new cards and connects them with UI elements.
 */
class CardManager : public Node, public ISerializable
{

private:
	int maxHandSize = 3;

	int maxManaPoints = 15;
	int currentManaPoints = 15;

	int selectedCard = -1;
	int selectedCardUp = 0;
	bool rowDown = true;

	shared_ptr<Player> player;

	std::vector<float> slotsY;

	/* it stores one copy of each type of a card that is currently unlocked */
	std::vector<shared_ptr<Card>> unlockedCards;
	std::vector<shared_ptr<CardUI>> unlockedCardDisplays;
	std::vector<shared_ptr<CardUI>> menuCardDisplays;

	/* all cards that are currently in hand, with each index corresponding with each slot */
	std::vector<shared_ptr<Card>> currentHand;

	/* this vector is used for saving currentHand when learning card */
	std::vector<shared_ptr<Card>> currentHandSaved;

	/* one CardUI for each card type */
	std::vector<shared_ptr<CardUI>> cardDisplays;

	shared_ptr<Card> learningCard;
	shared_ptr<Scene> cardScene;
	std::vector<shared_ptr<CardSlot>> slots;

	shared_ptr<Counter> manaCounter;

	shared_ptr<Icon> checkpointIcon;
	std::vector<shared_ptr<Icon>> slotIcons;

	bool menuOpen = false;

	/*
	 * @brief checks if currentHand is empty
	 * @returns bool: false if the card has at least one card
	 */
	bool IsEmpty();

	/*
	 * @brief helper functions for getting slot and cardUI nodes from scene
	 */
	void FindNodes(shared_ptr<Node> node);

	void UpdateManaUI();

	void MoveUnlockedCards();

	void AddCardUI(shared_ptr<CardUI> cardUI);

	void MoveSlots();

	void UpdateCardSelection();

	void AddCardToHand(int slot, shared_ptr<Card> card);
	void AddCardToHand(shared_ptr<Card> card);
	void RemoveCardFromHand(int slot);

	void Select(int slot=-1);


public:
	CardManager();
	~CardManager();

	/*
	 * @brief Initialization function that is called by EngineController
	 */
	void Init(shared_ptr<ResourceManager> rsm);

	/*
	 * @brief Getter for all currently Unlocked Cards
	 * @return vector<shared_ptr<Card>> : vector of all unlocked cards
	 */
	std::vector<shared_ptr<Card>> GetUnlockedCards();


	/*
	* @brief Getter for all cards currently in the Deck
	* @return vector<shared_ptr<Card>> : vector of all cards that are currently in the deck
	*/
	std::vector<shared_ptr<Card>> GetCurrentHand();

	/*
	 * @brief Sets the max number of cards that can be in hand
	 */
	void SetMaxHandSize(int value);

	/*
	 * @brief Returns the max number of cards that can be in hand
	 * @return int : max hand size
	 */ 
	int GetMaxHandSize();

	/*
	 * @brief Return Scene containing slots and cardUI's that is loaded in Init
	 * @return shared_ptr<Scene> : shared pointer to card scene
	 */
	std::shared_ptr<Scene> GetCardScene();

	/*
	 * @brief "factory" function for creating a card of certain type
	 */
	shared_ptr<Card> CreateCard(CardType type);

	/*
	 * @brief sets learningCard, when it is not null hand contains cards only of this type.
	 * @param shared_ptr<Card> : card that is going to be "learned"
	 */
	void LearnCard(shared_ptr<Card> card);
	
	/*
	 * @brief function that should be called when player reaches checkpoint.
	 */
	void ReachCheckpoint();
	
	/*
	 * @brief adds card to unlockedCards vector
	 * After that cards of this type can be added to the deck
	 * @param shared_ptr<Card> unlocked card
	 */
	void UnlockCard(shared_ptr<Card> card);

	/*
	 * @brief Uses card, and handles all related animations
	 * @param int : index of slot
	 */
	void UseCard(int index);

	/*
	 * @brief adds card to currentHand in specified slot, and handles all related animations
	 * @param int : index of slot
	 * @param shared_ptr<Card> : card to be added to hand
	 */
	void AddToHand(int slot, shared_ptr<Card> card);

	/*
	 * @brief Assigns player to all cards in deck, so they can use player-related functions
	 * @param shared_ptr<Player> : player to be assigned
	 */
	void AssignPlayer(shared_ptr<Player> player);

	void ToggleMenu();
	int getCurrentManaPoints() const { return currentManaPoints; }

	void Process() override;
	bool Input(InputEvent& event) override;

	std::string GetSerializeKey() const override;
	nlohmann::json Serialize() const override;
	void Deserialize(const nlohmann::json& data) override;
};



#endif // CARD_MANAGER
