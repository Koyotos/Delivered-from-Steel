#ifndef FE_BREAKABLE_WALL
#define FE_BREAKABLE_WALL
#include "include/Game/Objects/Platform.hpp"
class BreakableWall :
    public Platform
{
private:
    
public:
    BreakableWall(const unordered_map<string, std::any>&);

	void BreakWall();

};

#endif // FE_BREAKABLE_WALL
