#ifndef FE_CARD
#define FE_CARD

#include "include/Core/Object2D.hpp"
#include "include/Game/UI/CardUI.hpp"
#include "include/Game/CardType.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Core/Scene.hpp"

/*
 * @brief clas that represents card logic 
 */
class Card :
    public Object2D
{
private:
    CardType type;
    int maxEquippedDeck;
    int cardCost;
    bool used = false;

    bool DestroyAfterUsed = false;

    shared_ptr<CardUI> display;

	shared_ptr<Player> player;

public:
    Card(const unordered_map<string, std::any>&);
    Card(CardType type);

    /*
     * @brief function that returns card's UI representation
     * @return shared_ptr<CardUI> : CardUI object representing UI display of card
     */ 
    std::shared_ptr<CardUI> GetDisplay();

    /*
	 * @brief Set card's UI representation
	 * @param shared_ptr<CardUI> : CardUI object representing UI display of card
	 */ 
    void SetDisplay(std::shared_ptr<CardUI> value);

    /* 
     * @brief returns the type of card (eg. Dash, bounce etc)
     * @param CardType enum
     */ 
	CardType GetCardType();


    void Deactivate();

    /*
     * @brief function for checking if card can be used.
     * for example, is the player next to a wall
     * @return bool
     */ 
    bool CheckUse();

    /*
     * @brief function for executing cards logic.
     * CALL FUNCTIONS LIKE DASH, BOUNCE ETC IN THIS FUNCTION
     */
    void Use();

    int GetCardCost() const;

	void AssignPlayer(shared_ptr<Player> player);
};

#endif // FE_CARD
