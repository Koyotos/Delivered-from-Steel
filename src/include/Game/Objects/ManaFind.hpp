#ifndef FE_MANA_FIND
#define FE_MANA_FIND

#include "include/Game/Objects/PickUpAbstract.hpp"

class ManaFind : public PickUpAbstract {
private:
public:
    ManaFind();
    ManaFind(const unordered_map<string, std::any>& data);
    virtual ~ManaFind();

    void OnPickUp() override;

};
#endif // FE_MANA_FIND