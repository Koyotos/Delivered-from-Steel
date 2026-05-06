#ifndef FE_CARD
#define FE_CARD

#include "include/Core/Object2D.hpp"
#include "include/Game/UI/CardUI.hpp"

enum class CardType {
    Dash,
    Bounce,
    FeatherFalling,
    ExtraJump,
    WallSnap,
    WallJump
};



class Card :
    public Object2D
{
private:
    CardType type;
    int maxEquippedDeck;
    bool used = false;

    bool DestroyAfterUsed = false;

    std::shared_ptr<CardUI> display;

public:
    Card(const unordered_map<string, std::any>&);
    Card(CardType type);

    std::shared_ptr<CardUI> GetDisplay();
    void SetDisplay(std::shared_ptr<CardUI> value);

	CardType GetCardType();
   

    void select();

    void reject();

    bool IsNextToWall();

    bool IsNextToGround();

    void Deactivate();

    bool CheckUse();
    void Use();
};

#endif // FE_CARD
