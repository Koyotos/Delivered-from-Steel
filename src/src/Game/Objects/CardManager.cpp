#include "include/Game/Objects/CardManager.hpp"

std::vector<shared_ptr<Card>> CardManager::GetCurrentDeck()
{
	return currentDeck;
}

std::vector<shared_ptr<Card>> CardManager::GetCurrentHand()
{
	return currentHand;
}

std::vector<shared_ptr<Card>> CardManager::GetUnlockedCards()
{
	return unlockedCards;
}

void CardManager::SetDrawOnHandEmpty(bool value) 
{
	drawOnHandEmpty = value;
}

bool CardManager::GetDrawOnHandEmpty()
{
	return drawOnHandEmpty;
}

void CardManager::SetMaxDeckSize(int value)
{
	maxDeckSize = value;
}

int CardManager::GetMaxDeckSize()
{
	return maxDeckSize;
}

void CardManager::SetMaxHandSize(int value)
{
	maxHandSize = value;
}

int CardManager::GetMaxHandSize()
{
	return maxHandSize;
}

void CardManager::UnlockCard(std::shared_ptr<Card> card)
{
	unlockedCards.push_back(card);
}

bool CardManager::AddCardToDeck(std::shared_ptr<Card> card)
{
	if (currentDeck.size() <= maxDeckSize)
	{
		currentDeck.push_back(card);
		return true;
	}
	return false;
}

void CardManager::RemoveCardFromDeck(std::shared_ptr<Card> card)
{
	for (int i = 0; i < currentDeck.size(); i++)
	{
		// if(currentDeck[i]->GetCardType() == card->GetCardType())
	}
}

void CardManager::RemoveCardFromDeck(int index)
{
	if (index > currentDeck.size()) return;

	currentDeck.erase(currentDeck.begin() + index);
	currentDeck.shrink_to_fit(); // sprawdzic pozniej
	
}

void CardManager::DrawCardToHand(int slot)
{
	if (currentHand[slot] != nullptr) return;

	ShuffleDeck();

	std::shared_ptr<Card> newCard = currentDeck.back();
	currentHand.insert(currentHand.begin() + slot, newCard);
	// slots[slot]->SetCard(newCard->getUI());
	currentDeck.pop_back();

}

void CardManager::DrawCardsToHand()
{
	for (int i = 0; i < currentHand.size(); i++)
	{
		if (currentHand[i] == nullptr) DrawCardToHand(i);
	}
}

void CardManager::UseCard(int index)
{
	if (index > maxHandSize) return;
	if (currentHand[index] == nullptr) return;

	currentHand[index]->Use();
	slots[index]->RemoveCard();
	
	if (drawOnHandEmpty) DrawCardToHand(index);
	else if (IsEmpty()) DrawCardsToHand();

	

}

void CardManager::ShuffleDeck()
{
	return; //TBD
}

bool CardManager::IsEmpty()
{
	return currentHand.empty();
}
