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

