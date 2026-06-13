#ifndef FE_CARD_FIND
#define FE_CARD_FIND

#include "include/Game/Objects/PickUpAbstract.hpp"
#include "include/Game/CardType.hpp"

class CardFind : public PickUpAbstract {
private:
    CardType cardTypeToUnlock = CardType::WallJump;

public:
    CardFind();
    CardFind(const unordered_map<string, std::any>& data);
    virtual ~CardFind();

	void OnPickUp() override;

};
#endif // FE_CARD_FIND