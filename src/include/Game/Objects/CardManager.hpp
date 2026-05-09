#ifndef CARD_MANAGER
#define CARD_MANAGER

#include "include/Core/Node.hpp"
#include "include/Game/Objects/Card.hpp"
#include "include/Game/UI/CardSlot.hpp"
#include "include/ResourceManager/ResourceManager.hpp"


/*
 * @brief component class for handling all card-related operations.
 * It is created inside the EngineController's Init function.
 * It also creates new cards and connects them with UI elements.
 */
class CardManager : public Node
{

private:
	int maxDeckSize = 9;
	int maxHandSize = 3;
	bool drawOnHandEmpty = true;

	/* it stores one copy of each type of a card that is currently unlocked */
	std::vector<shared_ptr<Card>> unlockedCards;

	/* all cards that are in the deck, unshuffled */
	std::vector<shared_ptr<Card>> allDeckCards;

	/* all cards that are in the deck, when card is drawn it is removed */
	std::vector<shared_ptr<Card>> currentDeck;

	/* all cards that are currently in hand, with each index corresponding with each slot */
	std::vector<shared_ptr<Card>> currentHand;

	/* one CardUI for each card type */
	std::vector<shared_ptr<CardUI>> cardDisplays;

	shared_ptr<Card> learningCard;
	shared_ptr<Scene> cardScene;
	std::vector<shared_ptr<CardSlot>> slots;

	/*
	 * @brief helper function for shuffling the currentDeck
	 */
	void ShuffleDeck();

	/*
	 * @brief helper function for sorting currentHand by cardType
	 */
	void SortHand();

	/*
	 * @brief checks if currentHand is empty
	 * @returns bool: false if the card has at least one card
	 */
	bool IsEmpty();

	/*
	 * @brief helper functions for getting slot and cardUI nodes from scene
	 */
	void FindNodes(shared_ptr<Node> node);

	/*
	 * @brief "factory" function for creating a card of certain type
	 */
	shared_ptr<Card> CreateCard(CardType type);


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
	 * @brief Getter for all Cards currently in deck.
	 * USE THIS FOR CHECKPOINT UI.	
	 * @return vector<shared_ptr<Card>> : vector of all cards in deck
	 */
	std::vector<shared_ptr<Card>> GetAllDeckCards();

	/*
	* @brief Getter for all cards currently in the Deck
	* @return vector<shared_ptr<Card>> : vector of all cards that are currently in the deck
	*/
	std::vector<shared_ptr<Card>> GetCurrentDeck();

	/*
	* @brief Getter for all cards currently in the Deck
	* @return vector<shared_ptr<Card>> : vector of all cards that are currently in the deck
	*/
	std::vector<shared_ptr<Card>> GetCurrentHand();

	/*
	 * @brief Setter for DrawOnHandEmpty.
	 * If true card is drawn only after the hand is empty;
	 */
	void SetDrawOnHandEmpty(bool value);

	/*
	 * @brief Getter for DrawOnHandEmpty
	 * @return bool : true if all cards are drawn after the hand is empty
	 */
	bool GetDrawOnHandEmpty();

	/*
	 * @brief Sets the max number of cards that can be in the deck
	 */
	void SetMaxDeckSize(int value);

	/*
	 * @brief Returns the max number of cards that can be in the deck
	 * @returns int : max deck size
	 */
	int GetMaxDeckSize();
	
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
	 * @brief sets learningCard, when it is not null it draws card only of this type.
	 * @param shared_ptr<Card> : card that is going to be "learned"
	 */
	void LearnCard(shared_ptr<Card> card);
	
	/*
	 * @brief function that handles all card-related checkpoint operations
	 */
	void ReachCheckpoint();
	
	/*
	 * @brief adds card to unlockedCards vector
	 * After that cards of this type can be added to the deck
	 * @param shared_ptr<Card> unlocked card
	 */
	void UnlockCard(shared_ptr<Card> card);

	/*
	 * @brief adds card tu current and allCards decks
	 * @param shared_ptr<Card> shared ptr to added card
	 */
	bool AddCardToDeck(shared_ptr<Card> card);

	/*
	 * @brief removes first card of type from allCardsDeck
	 * @param shared_ptr<Card> : type of card to be removed
	 */
	void RemoveCardFromDeck(shared_ptr<Card> card);

	/*
	 * @brief removes card from allCardsDeck by index
	 * @param int : index of card to be removed
	 */
	void RemoveCardFromDeck(int index);

	/*
	 * @brief draws card from currentDeck to currentHand
	 * @param int : index of slot 
	 */
	void DrawCardToHand(int slot);

	/*
	 * @brief draws card to all empty slots
	 */
	void DrawCardsToHand();

	/*
	 * @brief Uses card, and handles all related animations
	 * @param int : index of slot
	 */
	void UseCard(int index);

	/*
	 * @brief Adds all cards to current deck and shuffles it
	 */
	void RefreshCurrentDeck();

	void Process() override;
	bool Input(InputEvent& event) override;


};



#endif // CARD_MANAGER
