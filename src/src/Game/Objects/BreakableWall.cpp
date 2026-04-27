#include "include/Game/Objects/BreakableWall.hpp"

BreakableWall::BreakableWall(const unordered_map<string, std::any>& data)
    : Platform(data)
{
	objectType = ObjectType::BreakableWall;
}

void BreakableWall::BreakWall() {
	Disable();
	// You can add some visual effects or sounds here to indicate the wall breaking
}