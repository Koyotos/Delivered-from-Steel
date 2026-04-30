#ifndef FE_CARD
#define FE_CARD

#include "include/Core/Object2D.hpp"
#include "include/Game/UI/CardUI.hpp"


class Card :
    public Object2D
{
private:
    int maxEquippedDeck;
    bool used = false;

    bool DestroyAfterUsed = false;

    shared_ptr<CardUI> display;

public:
    Card(const unordered_map<string, std::any>&);

    //virtual void UseCard();

    //virtual void CheckUseCard();

    void select();

    void reject();

    bool IsNextToWall();

    bool IsNextToGround();

    void Deactivate();

    void Use();
};

#endif // FE_CARD
