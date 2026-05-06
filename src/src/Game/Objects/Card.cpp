#include "include/Game/Objects/Card.hpp"

Card::Card(const unordered_map<string, std::any>& data) : Object2D(data) {
}

void Card::Deactivate()
{
    
}

void Card::Use()
{
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