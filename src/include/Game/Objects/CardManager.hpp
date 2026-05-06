#ifndef CARD_MANAGER
#define CARD_MANAGER

#include "include/Core/Node.hpp"
#include "include/Game/Objects/Card.hpp"
#include "include/Game/UI/CardSlot.hpp"

class CardManager : public Node
{

private:
	int maxDeckSize = 9;
	int maxHandSize = 3;
	bool drawOnHandEmpty = true;

	std::vector<shared_ptr<Card>> unlockedCards;
	std::vector<shared_ptr<Card>> allDeckCards;
	std::vector<shared_ptr<Card>> currentDeck;
	std::vector<shared_ptr<Card>> currentHand;
	shared_ptr<Card> learningCard;

	std::vector<shared_ptr<CardSlot>> slots;

	void ShuffleDeck();
	void SortHand();
	bool IsEmpty();


public:
	CardManager();
	CardManager(const unordered_map<string, std::any>& data);
	~CardManager();

	std::vector<shared_ptr<Card>> GetUnlockedCards();
	std::vector<shared_ptr<Card>> GetCurrentDeck();
	std::vector<shared_ptr<Card>> GetCurrentHand();

	void SetDrawOnHandEmpty(bool value);
	bool GetDrawOnHandEmpty();

	void SetMaxDeckSize(int value);
	int GetMaxDeckSize();
	
	void SetMaxHandSize(int value);
	int GetMaxHandSize();

	void LearnCard(shared_ptr<Card> card);
	
	void ReachCheckpoint();
	
	void UnlockCard(shared_ptr<Card> card);
	bool AddCardToDeck(shared_ptr<Card> card);
	void RemoveCardFromDeck(shared_ptr<Card> card);
	void RemoveCardFromDeck(int index);
	void DrawCardToHand(int slot);
	void DrawCardsToHand();
	void UseCard(int index);
	void RefreshCurrentDeck();

	void Process() override;




};



#endif // CARD_MANAGER
