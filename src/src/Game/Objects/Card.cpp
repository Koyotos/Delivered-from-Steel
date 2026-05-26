#include "include/Game/Objects/Card.hpp"
#include "include/Globals/Globals.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

Card::Card(const unordered_map<string, std::any>& data) : Object2D(data) {
}

Card::Card(CardType type)
{
	this->type = type;
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
		return !player->isDead() && (player->CheckLeftWalled() || player->CheckRightWalled()) && !player->CheckGrounded() ;
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