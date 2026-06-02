#include "include/Game/Objects/LevelGateway.hpp"
#include "include/Globals/Globals.hpp"
#include "include/EngineController/EngineController.hpp"

LevelGateway::LevelGateway() = default;

LevelGateway::LevelGateway(const unordered_map<string, std::any>& data) : PhysicsNode(data) {
	if (data.find("targetLevel") != data.end()) {
		targetLevel = fromMap(std::string, "targetLevel", data);
	}
	if (data.find("isLoad") != data.end()) {
		isLoadTrigger = fromMap(bool, "isLoad", data);
	}
}

string LevelGateway::Type() {
	return "LevelGateway";
}

void LevelGateway::OnCollisionEnter(shared_ptr<Collider> other) {
	if (hasTriggered) return;

	auto owner = other ? other->GetOwner() : nullptr;
	if (!owner || owner->GetObjectType() != ObjectType::Player) return;

	auto& globals = Globals::GetGlobals();
	auto engine = globals.engineController;
	if (!engine) return;

	hasTriggered = true;

	string target = targetLevel;
	string active = engine->GetActiveLevelName();
	string prev = engine->GetPreviousLevelName();

	if (isLoadTrigger) {
		if (target == active) {
			return;
		}
		else if (target == prev) {
			engine->QueueSwapActiveAndPrevious();
		}
		else {
			engine->QueueStreamNextLevel(target);
		}
	}
	else {
		if (target == prev) {
			engine->QueueUnloadPreviousLevel();
		}
		else if (target == active) {
			engine->QueueSwapActiveAndPrevious();
			engine->QueueUnloadPreviousLevel();
		}
	}
}

void LevelGateway::OnCollisionExit(shared_ptr<Collider> other) {
	auto owner = other ? other->GetOwner() : nullptr;
	if (!owner || owner->GetObjectType() != ObjectType::Player) return;

	hasTriggered = false;
}