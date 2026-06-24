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
	for (auto& display : cardDisplays)
	{
		if (display->GetCardType() == card->GetCardType())
		{
			shared_ptr<CardUI> cardUI = display;
			cardUI->MoveTo(vec2(-300.0f, 800.0f), 0.01f);
			cardUI->SetVisible(true);
			unlockedCardDisplays.push_back(cardUI);
			return;
		}
	}
}

void CardManager::UseCard(int index)
{
	if (menuOpen)
	{
		Select(index);
		return;
	}
	if (index < 0 || index >= currentHand.size()) return;
	if (currentHand[index] == nullptr) return;
	if (!currentHand[index]->CheckUse()) return;
	if (currentManaPoints < currentHand[index]->GetCardCost()) return;

	currentHand[index]->Use();
	slots[index]->PlayUseAnimation();
	if (learningCard == nullptr) currentManaPoints -= currentHand[index]->GetCardCost();
	UpdateManaUI();

}



void CardManager::ReachCheckpoint()
{
	if (learningCard)
	{
		if (learningCard->GetCardType() == CardType::FeatherFalling) ffTooltip->Activate();
		learningCard = nullptr;
		for (int i = 0; i < currentHand.size(); i++)
		{
			slots[i]->RemoveCard();
			currentHand[i] = nullptr;
			if (currentHandSaved[i] != nullptr)
			{
				currentHand[i] = currentHandSaved[i];
				for (auto it = unlockedCardDisplays.begin(); it != unlockedCardDisplays.end(); ++it)
				{
					if ((*it)->GetCardType() == currentHandSaved[i]->GetCardType())
					{
						slots[i]->SetCard(*it);
						unlockedCardDisplays.erase(it);
						break;
					}
				}
			}
		}
		manaCounter->IsLearning(false);
	}

	currentManaPoints = maxManaPoints;
	UpdateManaUI();

}

void CardManager::LearnCard(shared_ptr<Card> card)
{
	learningCard = card;
	UnlockCard(learningCard);
	for (int i = 0; i < maxHandSize; i++) {
		currentHand[i] = CreateCard(learningCard->GetCardType());
		shared_ptr<CardUI> removed = slots[i]->RemoveCard();
		if (removed)
		{
			removed->MoveTo(vec2(-300.0f, 800.0f), 0.01f);
			removed->RotateTo(0.0f, 0.01f);
			unlockedCardDisplays.push_back(removed);
		}
		slots[i]->LearnCard(currentHand[i]->GetDisplay());
	}
	manaCounter->IsLearning(true);

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
	slotIcons.resize(3);
	manaCounter = make_shared<Counter>(nullptr, nullptr, maxManaPoints);

	SetInput(true);
	SetProcess(true);

}

void CardManager::Process()
{
	
}

bool CardManager::Input(InputEvent& event)
{
	if (!event.handled)
	{
		if (event.type == InputType::GAMEPAD_BUTTON && event.action == GLFW_PRESS)
		{
			switch (event.key)
			{
			case GLFW_GAMEPAD_BUTTON_X: UseCard(0); event.handled = true; break;
			case GLFW_GAMEPAD_BUTTON_Y: UseCard(1); event.handled = true; break;
			case GLFW_GAMEPAD_BUTTON_B: UseCard(2); event.handled = true; break;
			case GLFW_GAMEPAD_BUTTON_A: if (menuOpen) Select(); event.handled = true; break;
			case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: if (menuOpen) {
				ChangeSelection(false);
				UpdateCardSelection();
				event.handled = true;
			} break;
			case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: if (menuOpen) {
				ChangeSelection(true);
				UpdateCardSelection();
				event.handled = true;
			} break;
			case GLFW_GAMEPAD_BUTTON_DPAD_UP: if (menuOpen) {
				rowDown = !rowDown;
				selectedCard = 0;
				UpdateCardSelection();
				event.handled = true;
			} break;
			case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: if (menuOpen) {
				rowDown = !rowDown;
				selectedCard = 0;
				UpdateCardSelection();
				event.handled = true;
			} break;
			}
		}
		if (event.type == InputType::GAMEPAD_AXIS)
		{
			if (menuOpen && event.key == GLFW_GAMEPAD_AXIS_LEFT_X)
			{
				if (event.valueX > 0.5f && !axisHeld) {
					ChangeSelection(false);
					UpdateCardSelection();
					axisHeld = true;
					event.handled = true;
				}
				else if (event.valueX < -0.5f && !axisHeld) {
					ChangeSelection(true);
					UpdateCardSelection();
					axisHeld = true;
					event.handled = true;
				}
				else if (event.valueX > -0.5f && event.valueX < 0.5f && axisHeld) {
					axisHeld = false;
					event.handled = true;
				}
			}
			if (menuOpen && event.key == GLFW_GAMEPAD_AXIS_LEFT_Y)
			{
				if (event.valueX > 0.5f && !axisHeldY) {
					rowDown = !rowDown;
					selectedCard = 0;
					UpdateCardSelection();
					axisHeldY = true;
					event.handled = true;
				}
				else if (event.valueX < -0.5f && !axisHeldY) {
					rowDown = !rowDown;
					selectedCard = 0;
					UpdateCardSelection();
					axisHeldY = true;
					event.handled = true;
				}
				else if (event.valueX > -0.5f && event.valueX < 0.5f && axisHeldY) {
					axisHeldY = false;
					event.handled = true;
				}
			}
		}
		if (event.type == InputType::KEYBOARD && event.action == GLFW_PRESS)
		{
			switch (event.key)
			{
			case GLFW_KEY_J: UseCard(0); event.handled = true; break;
			case GLFW_KEY_K: UseCard(1); event.handled = true; break;
			case GLFW_KEY_L: UseCard(2); event.handled = true; break;
			case GLFW_KEY_SPACE: if (menuOpen) Select(); event.handled = true; break;
			case GLFW_KEY_D: if (menuOpen) {
				ChangeSelection(false);
				UpdateCardSelection();
				event.handled = true;
			} break;
			case GLFW_KEY_A: if (menuOpen) {
				ChangeSelection(true);
				UpdateCardSelection();
				event.handled = true;
			} break;
			case GLFW_KEY_W: if (menuOpen) {
				rowDown = !rowDown;
				selectedCard = 0;
				UpdateCardSelection();
				event.handled = true;
			} break;
			case GLFW_KEY_S: if (menuOpen) {
				rowDown = !rowDown;
				selectedCard = 0;
				UpdateCardSelection();
				event.handled = true;
			} break;
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

	for (int i = 0; i < slots.size(); i++)
	{
		slots[i]->SetIcon(slotIcons[i]);
		slots[i]->Init();
	}

	manaCounter->UpdateMaxVal(maxManaPoints);


	UnlockCard(CreateCard(CardType::WallJump));
	UnlockCard(CreateCard(CardType::Dash));


	UpdateManaUI();

	firstCheckpoint = true;

	
}

void CardManager::FindNodes(shared_ptr<Node> node)
{
	if (!node) return;

	if (node->Type() == "CardSlot") {
		shared_ptr<CardSlot> cast = static_pointer_cast<CardSlot>(node);
		slots.push_back(cast);
		std::sort(slots.begin(), slots.end(), [](const std::shared_ptr<CardSlot>& a, const std::shared_ptr<CardSlot>& b) {return a->GetTransform().GetGlobal()[3].x < b->GetTransform().GetGlobal()[3].x;});

	}
	else if (node->Type() == "TextUI") {
		shared_ptr<TextUI> cast = static_pointer_cast<TextUI>(node);
		manaCounter->SetText(cast);
	}
	else if (node->Type() == "CardUI") {
		shared_ptr<CardUI> cast = static_pointer_cast<CardUI>(node);
		cardDisplays.push_back(cast);
	}
	else if (node->Type() == "Icon") {
		shared_ptr<Icon> cast = static_pointer_cast<Icon>(node);
		if (cast->GetName() == "mana_wheel") manaCounter->SetIcon(cast);
		else if (cast->GetName() == "mana_icon") manaCounter->AddManaIcon(cast);
		else if (cast->GetName() == "infinity_icon") manaCounter->SetInfinityIcon(cast);
		else if (cast->GetName() == "checkpoint_bg") checkpointBackground = cast;
		else if (cast->GetName() == "button_x") slotIcons[0] = cast;
		else if (cast->GetName() == "button_y") slotIcons[1] = cast;
		else if (cast->GetName() == "button_b") slotIcons[2] = cast;
	}
	else if (node->Type() == "Tooltip")
	{
		shared_ptr<Tooltip> cast = static_pointer_cast<Tooltip>(node);
		if (cast->GetName() == "ff") ffTooltip = cast;
		else if (cast->GetName() == "checkpoint") checkpointTooltip = cast;
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
	if (player) newCard->AssignPlayer(player);
	return newCard;
}

void CardManager::AssignPlayer(shared_ptr<Player> player)
{
	for (auto& card : unlockedCards) if (card) card->AssignPlayer(player);
	for (auto& card : currentHand) if (card) card->AssignPlayer(player);
	this->player = player;
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
	unlockedCards.clear();
	unlockedCardDisplays.clear();
	for (int i = 0; i < slots.size(); i++) {
		slots[i]->RemoveCard();
		currentHand[i] = nullptr;
		currentHandSaved[i] = nullptr;
	}

	if (data.contains("unlockedCards")) {
		auto loadedUnlocked = data["unlockedCards"].get<std::vector<int>>();
		for (int typeVal : loadedUnlocked) {
			UnlockCard(CreateCard(static_cast<CardType>(typeVal)));
		}
	}

	if (data.contains("currentHand")) {
		auto loadedHand = data["currentHand"].get<std::vector<int>>();

		for (int i = 0; i < loadedHand.size() && i < slots.size(); i++) {
			if (loadedHand[i] != -1) {
				CardType type = static_cast<CardType>(loadedHand[i]);
				auto newCard = CreateCard(type);
				currentHand[i] = newCard;
				currentHandSaved[i] = newCard;
				for (auto it = unlockedCardDisplays.begin(); it != unlockedCardDisplays.end(); ++it) {
					if ((*it)->GetCardType() == type) {
						slots[i]->SetCard(*it);
						unlockedCardDisplays.erase(it);
						break;
					}
				}
			}
		}
	}
	selectedCard = 0;
	UpdateCardSelection();
	MoveUnlockedCards();
	MoveSlots();
	if (!menuOpen && checkpointBackground) {
		checkpointBackground->FinishAllTweens();
		checkpointBackground->MoveTo(vec2(0.0f, -1080.0f), 0.01f, EaseType::Linear);
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
}

void CardManager::ToggleMenu()
{
	menuOpen = !menuOpen;
	if (menuOpen) {
		selectedCard = 0;
		rowDown = true;
		player->SetPhysics(false);
		player->StopAllLoopingAudio();
		UpdateCardSelection();	
		checkpointBackground->ClearAllTweens();
		checkpointBackground->MoveTo(vec2(0.0f, 0.0f), 0.5f, EaseType::OutQuad);
		MoveUnlockedCards();
		MoveSlots();
		if (firstCheckpoint)
		{
			checkpointTooltip->Activate();
		}
	}
	else
	{
		checkpointBackground->FinishAllTweens();
		checkpointBackground->MoveTo(vec2(0.0f, -1080.0f), 0.5f, EaseType::InQuad);
		MoveUnlockedCards();
		MoveSlots();
		player->SetPhysics(true);
		checkpointTooltip->Deactivate();
	}
}

void CardManager::MoveUnlockedCards()
{

	if (menuOpen) {
		int count = unlockedCardDisplays.size();

		for (int i = 0; i < count; i++) {
			float targetX = 100.0f + (i + 1) * (1720.0f / (count + 1));
			unlockedCardDisplays[i]->MoveTo(vec2(targetX - 75.0f, 220.0f), 0.5f, EaseType::OutQuad);
			unlockedCardDisplays[i]->Tint(vec3(0.75f, 0.75f, 0.75f), 0.01f, EaseType::OutQuad);
		}
	}
	else {
		for (auto& display : unlockedCardDisplays) {
			display->FinishAllTweens();
			display->MoveTo(vec2(-300.0f, 800.0f), 0.5f, EaseType::InOutSine);
			display->ScaleTo(vec2(2.7f, 2.7f), 0.5f, EaseType::Linear);
			display->Tint(vec3(1.0f, 1.0f, 1.0f), 0.5f, EaseType::InOutSine);
		}
	}
}



void CardManager::MoveSlots()
{
	if (menuOpen)
		for (int i = 0; i < slots.size(); i++) {
			slots[i]->ShowSlot(0.5f);
		}
	else
	{
		for (int i = 0; i < slots.size(); i++) {
			slots[i]->HideSlot(0.5f);
		}
	}
}

void CardManager::SetMaxMana(int value)
{
	maxManaPoints = value;
	currentManaPoints = value;
	manaCounter->UpdateMaxVal(value);
	manaCounter->UpdateValue(value);
	UpdateManaUI();
}

void CardManager::AddMaxMana(int value)
{
	SetMaxMana(maxManaPoints + value);
}

void CardManager::UpdateCardSelection()
{

	for (int i = 0; i < unlockedCardDisplays.size(); i++) {
		vec2 baseScale = vec2(2.7f, 2.7f);
		float cardW = 75.0f * baseScale.x;
		float cardH = 100.0f * baseScale.y;
		float baseX = 100.0f + (i + 1) * (1720.0f / (unlockedCardDisplays.size() + 1));
		float baseY = 220.0 ;

		if (i == selectedCard && rowDown) {
			unlockedCardDisplays[i]->ScaleTo(baseScale * 1.2f, 0.2f, EaseType::InOutSine);
			unlockedCardDisplays[i]->MoveTo(vec2(baseX - 75.0f - cardW * 0.05f, baseY - cardH * 0.05f), 0.2f, EaseType::InOutSine);
			unlockedCardDisplays[i]->Tint(vec3(1.0f, 1.0f, 1.0f), 0.2f, EaseType::InOutSine);
		}
		else {
			unlockedCardDisplays[i]->ScaleTo(baseScale, 0.2f, EaseType::InOutSine);
			unlockedCardDisplays[i]->MoveTo(vec2(baseX-75.0f, baseY), 0.2f, EaseType::InOutSine);
			unlockedCardDisplays[i]->Tint(vec3(0.75f, 0.75f, 0.75f), 0.2f, EaseType::InOutSine);
		}
	}
	for (int i = 0; i < slots.size(); i++) {
		if (i == selectedCard && !rowDown) {
			slots[i]->SetCardTint(vec3(1.0f, 1.0f, 1.0f));
			slots[i]->ScaleCardTo(vec2(1.2f, 1.2f), 0.2f, EaseType::InOutSine);
		}
		else {
			slots[i]->SetCardTint(vec3(0.75f, 0.75f, 0.75f));
			slots[i]->ScaleCardTo(vec2(1.0f, 1.0f), 0.2f, EaseType::InOutSine);
		}
	}
}

void CardManager::Select(int slot)
{
	if (!menuOpen) return;
	if (rowDown && selectedCard >= 0)
	{
		shared_ptr<Card> found = nullptr;
		for (auto& card : unlockedCards) {
			if (card->GetCardType() == unlockedCardDisplays[selectedCard]->GetCardType()) {
				found = card;
				break;
			}
		}
		switch (slot)
		{
		case 0: AddCardToHand(0, found); break;
		case 1: AddCardToHand(1, found); break;
		case 2: AddCardToHand(2, found); break;
		default: AddCardToHand(found); break;

		}
	}
	else if (!rowDown && selectedCard >= 0)
	{
		RemoveCardFromHand(selectedCard);
		
	}

}
void CardManager::AddCardToHand(shared_ptr<Card> card)
{
	for (int i = 0; i < currentHand.size(); i++) {
		if (currentHand[i] == nullptr) {
			AddCardToHand(i, card);
			return;
		}
	}
}

void CardManager::AddCardToHand(int slot, shared_ptr<Card> card)
{
	if (slot < 0 || slot >= (int)currentHand.size()) return;

	if (currentHand[slot] != nullptr && currentHand[slot]->GetDisplay() != nullptr) {
		RemoveCardFromHand(slot);
	}

	currentHand[slot] = card;
	currentHandSaved[slot] = card;

	auto& disp = unlockedCardDisplays[selectedCard];
	slots[slot]->SetCard(disp);
	unlockedCardDisplays.erase(unlockedCardDisplays.begin() + selectedCard);

	for (int i = 0; i < unlockedCardDisplays.size(); i++) {
		if (unlockedCardDisplays[i]->GetCardType() == card->GetCardType()) {
			unlockedCardDisplays.erase(unlockedCardDisplays.begin() + i);
			break;
		}
	}

	selectedCard = std::min(selectedCard, (int)unlockedCardDisplays.size() - 1);
	if (menuOpen) UpdateCardSelection();
}

void CardManager::RemoveCardFromHand(int slot)
{
	if (slot < 0 || slot >= (int)currentHand.size()) return;
	if (currentHand[slot] == nullptr) return;

	// zwr�� display z powrotem do menu
	shared_ptr<CardUI> returning = slots[slot]->RemoveCard();
	if (returning) {
		unlockedCardDisplays.push_back(returning);
		returning->RotateTo(0.0f, 0.1f, EaseType::InOutSine);
	}

	currentHand[slot] = nullptr;
	currentHandSaved[slot] = nullptr;

	UpdateCardSelection();
}

void CardManager::ChangeSelection(bool left)
{
	if (!left)
	{
		selectedCard = (selectedCard + 1) % (rowDown ? unlockedCardDisplays.size() : slots.size());
	}
	else
	{
		if (selectedCard == 0) selectedCard = (rowDown ? unlockedCardDisplays.size() : slots.size()) - 1;
		else selectedCard = (selectedCard - 1) % (rowDown ? unlockedCardDisplays.size() : slots.size());
	}
}


