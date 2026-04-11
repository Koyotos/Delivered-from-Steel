#ifndef FE_CARD
#define FE_CARD

#include "include/Core/Object2D.hpp"
class Card :
    public Object2D
{
public:
    Card(const unordered_map<string, std::any>&);
};

#endif // FE_CARD
