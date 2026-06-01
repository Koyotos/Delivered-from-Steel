#include "include/Game/Objects/CardManager.hpp"
#include "include/Globals/Globals.hpp"
#include <random>

#include "GLFW/glfw3.h"

std::vector<shared_ptr<Card>> CardManager::GetCurrentHand()
{
	return currentHand;
}

std::vector<shared_ptr<Card>> CardManager::GetUnlockedCards()
{
	return unlockedCards;
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

void CardManager::UseCard(int index)
{
	if (index < 0 || index >= currentHand.size()) return;
	if (currentHand[index] == nullptr) return;
	if (!currentHand[index]->CheckUse()) return;
	if (currentManaPoints < currentHand[index]->GetCardCost()) return;

	currentHand[index]->Use();
	slots[index]->PlayUseAnimation();
	if (learningCard == nullptr) currentManaPoints -= currentHand[index]->GetCardCost();
	UpdateManaUI();

}

void CardManager::AddToHand(int slot, shared_ptr<Card> card)
{
	if (slot < 0 || slot >= currentHand.size()) return;
	currentHand[slot] = card;
	currentHandSaved[slot] = card;
	slots[slot]->SetCard(card->GetDisplay());
}


void CardManager::ReachCheckpoint()
{
	learningCard = nullptr;
	currentHand = currentHandSaved;
	currentManaPoints = maxManaPoints;
	UpdateManaUI();
}

void CardManager::LearnCard(shared_ptr<Card> card)
{
	learningCard = card;
	unlockedCards.push_back(card);
	currentHandSaved = currentHand;
	for (int i = 0; i < maxHandSize; i++) {
		currentHand[i] = learningCard;
	}

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
	currentHand.resize(maxHandSize, nullptr);
	currentHandSaved.resize(maxHandSize, nullptr);
	manaCounter = make_shared<Counter>(nullptr, nullptr, maxManaPoints);

}

void CardManager::Process()
{
	return;
}

bool CardManager::Input(InputEvent& event)
{
	if(!event.handled)
	{
		if (event.type == InputType::GAMEPAD_BUTTON && event.action == GLFW_PRESS)
		{
			switch (event.key)
			{
			case GLFW_GAMEPAD_BUTTON_X: UseCard(0); event.handled = true; break;
			case GLFW_GAMEPAD_BUTTON_Y: UseCard(1); event.handled = true; break;
			case GLFW_GAMEPAD_BUTTON_B: UseCard(2); event.handled = true; break;
			}
		}
		if (event.type == InputType::KEYBOARD && event.action == GLFW_PRESS)
		{
			switch (event.key)
			{
			case GLFW_KEY_J: UseCard(0); event.handled = true; break;
			case GLFW_KEY_K: UseCard(1); event.handled = true; break;
			case GLFW_KEY_L: UseCard(2); event.handled = true; break;
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

	UnlockCard(CreateCard(CardType::WallJump));
	UnlockCard(CreateCard(CardType::Dash));
	UnlockCard(CreateCard(CardType::WallSnap));
	UnlockCard(CreateCard(CardType::DoubleJump));
	UnlockCard(CreateCard(CardType::FeatherFalling));

	AddToHand(0, CreateCard(CardType::WallJump));
	AddToHand(1, CreateCard(CardType::Dash));

	ReachCheckpoint();

}

void CardManager::FindNodes(shared_ptr<Node> node)
{
	// POPRAWIC TA FUNKCJE POZNIEJ


	if (auto cast = dynamic_pointer_cast<CardSlot>(node)) {
		slots.push_back(cast);
		std::sort(slots.begin(), slots.end(), [](const std::shared_ptr<CardSlot>& a, const std::shared_ptr<CardSlot>& b) {return a->GetTransform().GetGlobal()[3].x < b->GetTransform().GetGlobal()[3].x;});

	}
	else if (auto cast = dynamic_pointer_cast<TextUI>(node)) {
		manaCounter->SetText(cast);
	}
	else if (auto cast = dynamic_pointer_cast<CardUI>(node)) {
		cardDisplays.push_back(cast);
	}
	else if (auto cast = dynamic_pointer_cast<Icon>(node)) {
		if (cast->GetName() == "mana_wheel") manaCounter->SetIcon(cast);
		else if (cast->GetName() == "checkpoint_bg") checkpointIcon = cast;
	}


	for (auto& k : node->GetChildren()) {
		FindNodes(k);
	}
}

std::shared_ptr<Scene> CardManager::GetCardScene()
{
	return cardScene;
}

shared_ptr<Card> CardManager::CreateCard(CardType type)
{
	shared_ptr<Card> newCard = make_shared<Card>(type);
	for (auto& ui : cardDisplays) {
		if (ui->GetCardType() == type) {
			newCard->SetDisplay(make_shared<CardUI>(*ui));
			break;
		}
	}
	return newCard;
}

void CardManager::AssignPlayer(shared_ptr<Player> player)
{
	for (auto& card : unlockedCards) if (card) card->AssignPlayer(player);
	for (auto& card : currentHand) if (card) card->AssignPlayer(player);
}


std::string CardManager::GetSerializeKey() const {
	return "card_manager";
}

nlohmann::json CardManager::Serialize() const {
	nlohmann::json j;

	std::vector<int> unlocked;
	for (const auto& card : unlockedCards) {
		unlocked.push_back(static_cast<int>(card->GetCardType()));
	}
	j["unlockedCards"] = unlocked;

	std::vector<int> hand;
	for (const auto& card : currentHand) {
		if (card != nullptr) {
			hand.push_back(static_cast<int>(card->GetCardType()));
		}
		else {
			hand.push_back(-1);
		}
	}
	j["currentHand"] = hand;

	return j;
}

void CardManager::Deserialize(const nlohmann::json& data) {
	if (data.contains("unlockedCards")) {
		auto loadedUnlocked = data["unlockedCards"].get<std::vector<int>>();
		unlockedCards.clear();
		for (int typeVal : loadedUnlocked) {
			unlockedCards.push_back(CreateCard(static_cast<CardType>(typeVal)));
		}
	}

	if (data.contains("currentHand")) {
		auto loadedHand = data["currentHand"].get<std::vector<int>>();
		currentHand.clear();
		currentHand.resize(maxHandSize, nullptr);

		for (int i = 0; i < loadedHand.size() && i < slots.size(); i++) {
			if (loadedHand[i] != -1) {
				auto newCard = CreateCard(static_cast<CardType>(loadedHand[i]));
				currentHand[i] = newCard;
				slots[i]->SetCard(newCard->GetDisplay());
			}
			else {
				currentHand[i] = nullptr;
				slots[i]->RemoveCard();
			}
		}
	}
}

void CardManager::UpdateManaUI()
{
	for (int i = 0; i < slots.size(); i++) {
		if (currentHand[i] != nullptr) {
			if (currentManaPoints < currentHand[i]->GetCardCost()) {
				slots[i]->SetCardTint(vec3(0.3f, 0.3f, 0.3f)); 
			}
			else {
				slots[i]->SetCardTint(vec3(1.0f, 1.0f, 1.0f)); 
			}
		}
	}
	manaCounter->UpdateValue(currentManaPoints);

	// if learning card TBD
}

void CardManager::ToggleMenu()
{
	menuOpen = !menuOpen;
	if (menuOpen) {
		checkpointIcon->FinishAllTweens();
		checkpointIcon->MoveTo(vec2(checkpointIcon->GetTransform().GetTranslation().x, checkpointIcon->GetTransform().GetTranslation().y + 750), 0.5f, EaseType::OutQuad);
	}
	else
	{
		checkpointIcon->FinishAllTweens();
		checkpointIcon->MoveTo(vec2(checkpointIcon->GetTransform().GetTranslation().x, checkpointIcon->GetTransform().GetTranslation().y - 750), 0.5f, EaseType::InQuad);
	}
}