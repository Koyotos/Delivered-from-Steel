#include "include/Game/Objects/BreakableWall.hpp"
#include "include/Globals/Globals.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

BreakableWall::BreakableWall(const unordered_map<string, std::any>& data)
	: Platform(data)
{
	objectType = ObjectType::BreakableWall;
}

void BreakableWall::BreakWall() {
	std::string id = this->GetSaveID();
	if (!id.empty()) {
		auto& globals = Globals::GetGlobals();
		if (globals.worldStateManager) {
			std::string currentLevel = globals.activeLevelName;
			globals.worldStateManager->MarkAsDestroyed(currentLevel, id);
		}
	}
	Disable();
	// You can add some visual effects or sounds here to indicate the wall breaking
}