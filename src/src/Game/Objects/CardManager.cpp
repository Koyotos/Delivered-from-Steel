#include "include/Game/Objects/CardManager.hpp"
#include "include/Globals/Globals.hpp"
#include <random>

#include "GLFW/glfw3.h"

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
	if (allDeckCards.size() < maxDeckSize)
	{
		allDeckCards.push_back(card);
		currentDeck.push_back(card);
		return true;
	}
	return false;
}

void CardManager::RemoveCardFromDeck(std::shared_ptr<Card> card)
{
	for (int i = 0; i < allDeckCards.size(); i++)
	{
		if (allDeckCards[i]->GetCardType() == card->GetCardType()) currentDeck.erase(currentDeck.begin() + i);
	}
}

void CardManager::RemoveCardFromDeck(int index)
{
	if (index > currentDeck.size()) return;

	allDeckCards.erase(allDeckCards.begin() + index);
	allDeckCards.shrink_to_fit();

}

void CardManager::DrawCardToHand(int slot)
{
	if (currentDeck.empty()) return;
	if (learningCard != nullptr)
	{
		currentHand[slot] = learningCard;
		slots[slot]->SetCard(learningCard->GetDisplay());
		return;
	}

	ShuffleDeck();

	std::shared_ptr<Card> newCard = currentDeck.back();
	currentHand[slot] = newCard;
	slots[slot]->SetCard(newCard->GetDisplay());
	currentDeck.pop_back();

}

void CardManager::DrawCardsToHand()
{
	for (int i = 0; i < slots.size(); i++)
	{
		DrawCardToHand(i);
	}
}

void CardManager::UseCard(int index)
{
	if (index < 0 || index >= currentHand.size()) return;
	if (currentHand[index] == nullptr) return;
	if (!currentHand[index]->CheckUse()) return;

	currentHand[index]->Use();
	currentHand[index] = nullptr;
	slots[index]->RemoveCard();

	if (drawOnHandEmpty)
	{
		DrawCardToHand(index);
	}
	else if (IsEmpty())
	{
		DrawCardsToHand();
	}
}


void CardManager::ReachCheckpoint()
{
	learningCard = nullptr;
	RefreshCurrentDeck();
}

void CardManager::LearnCard(shared_ptr<Card> card)
{
	learningCard = card;
	DrawCardsToHand();
}

void CardManager::RefreshCurrentDeck()
{
	currentDeck = allDeckCards;
	ShuffleDeck();
	DrawCardsToHand();
}

void CardManager::ShuffleDeck()
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(currentDeck.begin(), currentDeck.end(), g);
}

bool CardManager::IsEmpty()
{
	for (auto& card : currentHand)
		if (card != nullptr)
			return false;
	return true;
}

CardManager::CardManager()
{
}

void CardManager::Process()
{
	return;
}

bool CardManager::Input(InputEvent& event)
{
	if(!event.handled)
	{
		if (event.type == InputType::GAMEPAD_BUTTON)
		{
			switch (event.key)
			{
			case GLFW_GAMEPAD_BUTTON_X: UseCard(0); break;
			case GLFW_GAMEPAD_BUTTON_Y: UseCard(1); break;
			case GLFW_GAMEPAD_BUTTON_B: UseCard(2); break;
			}
		}
		if (event.type == InputType::KEYBOARD)
		{
			switch (event.key)
			{
			case GLFW_KEY_J: UseCard(0); break;
			case GLFW_KEY_K: UseCard(1); break;
			case GLFW_KEY_L: UseCard(2); break;
			}
		}
	}
	return false;
}

CardManager::~CardManager()
{
}

void CardManager::Init(shared_ptr<ResourceManager> rsm)
{
	cardScene = rsm->LoadScene("res/scenes/cardsUI.json");
	shared_ptr<Node> root = cardScene->GetRoot();
	FindNodes(root);

}

void CardManager::FindNodes(shared_ptr<Node> node)
{
	if (auto cast = dynamic_pointer_cast<CardSlot>(node)) {
		slots.push_back(cast);
	}
	else if (auto cast = dynamic_pointer_cast<CardUI>(node)) {
		cardDisplays.push_back(cast);
	}

	for (auto& k : node->GetChildren()) {
		FindNodes(k);
	}
}

std::shared_ptr<Scene> CardManager::GetCardScene()
{
	return cardScene;
}
