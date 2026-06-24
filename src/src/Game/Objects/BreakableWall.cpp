#include "include/Game/Objects/BreakableWall.hpp"
#include "include/Globals/Globals.hpp"
#include "include/SaveManager/WorldStateManager.hpp"
#include "include/AudioManager/AudioManager.hpp" 

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
	if (auto aum = Globals::GetGlobals().audioManager) {
		aum->PlaySound3D("Break_Wall", GetTransform().GetTranslation(), 1.0f, 1.0f);
	}
	Disable();
	// You can add some visual effects or sounds here to indicate the wall breaking
}