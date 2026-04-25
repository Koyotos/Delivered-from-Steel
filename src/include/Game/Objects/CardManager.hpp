#ifndef CARD_MANAGER
#define CARD_MANAGER

#include "include/Core/Node.hpp"
#include "include/Game/Objects/Card.hpp"

class CardManager : public Node
{

private:
	int maxDeckSize = 9;
	int maxHandSize = 3;
	bool drawOnHandEmpty = true;

	std::vector<shared_ptr<Card>> unlockedCards;
	std::vector<shared_ptr<Card>> currentDeck;
	std::vector<shared_ptr<Card>> currentHand;

	void ShuffleDeck();
	void SortHand();


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
	

	void UnlockCard(shared_ptr<Card> card);
	bool AddCard(shared_ptr<Card> card);
	void RemoveCard(shared_ptr<Card> card);
	void RemoveCard(int index);
	void DrawCard(int slot);
	void DrawCard();
	void UseCard(int index);

	void Process() override;




};



#endif // CARD_MANAGER
