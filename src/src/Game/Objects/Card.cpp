#include "include/Game/Objects/Card.hpp"
#include "include/Globals/Globals.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

Card::Card(const unordered_map<string, std::any>& data) : Object2D(data) {
}

Card::Card(CardType type)
{
	this->type = type;
	switch (type)
	{
		case CardType::Dash:
			cardCost = 2;
			break;
		case CardType::Bounce:
			cardCost = 3;
			break;
		case CardType::FeatherFalling:
			cardCost = 1;
			break;
		case CardType::DoubleJump:
			cardCost = 4;
			break;
		case CardType::WallJump:
			cardCost = 3;
			break;
		case CardType::WallSnap:
			cardCost = 3;
			break;
	}
}

void Card::Deactivate()
{

}

bool Card::CheckUse()
{

	switch (type)
	{
	case CardType::Dash:
		return !player->isDead();
	case CardType::Bounce:
		return !player->isDead();
	case CardType::FeatherFalling:
		return !player->isDead();
	case CardType::DoubleJump:
		return !player->isDead() && !player->CheckGrounded();
	case CardType::WallJump:
		return !player->isDead() && player->CheckWallJump();
	case CardType::WallSnap:
		return !player->isDead() && player->CheckWallSnap();
	default:
		return true;
	}
	return true;
}

void Card::Use()
{
	if (!CheckUse()) return;

	switch (type)
	{
	case CardType::Dash:
		player->ExecuteDash();
		break;
	case CardType::Bounce:
		player->ExecuteBounce();
		break;
	case CardType::FeatherFalling:
		player->ExecuteFeatherFalling();
		break;
	case CardType::DoubleJump:
		player->ExecuteDoubleJump();
		break;
	case CardType::WallJump:
		player->ExecuteWallJump();
		break;
	case CardType::WallSnap:
		player->ExecuteWallSnap();
	}


	if (DestroyAfterUsed)
	{
		std::string id = this->GetSaveID();
		if (!id.empty()) {
			auto& globals = Globals::GetGlobals();
			if (globals.worldStateManager) {
				std::string currentLevel = globals.activeLevelName;
				globals.worldStateManager->MarkAsDestroyed(currentLevel, id);
			}
		}
		//TODO: destroy card
		return;
	}
	used = true;
	Deactivate();
}

void Card::SetDisplay(std::shared_ptr<CardUI> value)
{
	display = value;
}

std::shared_ptr<CardUI> Card::GetDisplay()
{
	return display;
}

CardType Card::GetCardType()
{
	return type;
}

void Card::AssignPlayer(shared_ptr<Player> player)
{
	this->player = player;
}

int Card::GetCardCost() const
{
	return cardCost;
}