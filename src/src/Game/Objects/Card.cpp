#include "include/Game/Objects/Card.hpp"

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
        return true;
    case CardType::Bounce:
        //return !player->CheckGrounded();
        return true;
    case CardType::FeatherFalling:
        return true;
    case CardType::DoubleJump:
        return !player->CheckGrounded();
    case CardType::WallJump:
        return (player->CheckLeftWalled() || player->CheckRightWalled()) && !player->CheckGrounded();
    case CardType::WallSnap:
        return true;
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
    }


    if (DestroyAfterUsed)
    {
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