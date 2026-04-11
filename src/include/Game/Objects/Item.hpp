#ifndef FE_ITEM
#define FE_ITEM

#include "include/Core/Object3D.hpp"
class Item :
    public Object3D
{
public:
	Item(const unordered_map<string, std::any>&);
};

#endif // FE_ITEM
