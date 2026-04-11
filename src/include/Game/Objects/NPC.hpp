#ifndef FE_NPC
#define FE_NPC

#include "include/Core/Object2D.hpp"
class NPC :
    public Object2D
{
    public:
		NPC(const unordered_map<string, std::any>&);
};

#endif // FE_NPC
