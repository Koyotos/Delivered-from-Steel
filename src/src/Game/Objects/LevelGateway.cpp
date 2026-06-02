#include "include/Game/Objects/LevelGateway.hpp"
#include "include/Globals/Globals.hpp"
#include "include/EngineController/EngineController.hpp"

LevelGateway::LevelGateway() = default;

LevelGateway::LevelGateway(const unordered_map<string, std::any>& data) : PhysicsNode(data) {
	if (data.find("targetLevel") != data.end()) {
		targetLevel = fromMap(std::string, "targetLevel", data);
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
	if (target == engine->GetActiveLevelName()) {
		engine->QueueUnloadPreviousLevel();
	}
	else if (target == engine->GetPreviousLevelName()) {
		engine->QueueSwapActiveAndPrevious();
	}
	else {
		engine->QueueStreamNextLevel(target);
	}
}

void LevelGateway::OnCollisionExit(shared_ptr<Collider> other) {
	auto owner = other ? other->GetOwner() : nullptr;
	if (!owner || owner->GetObjectType() != ObjectType::Player) return;

	hasTriggered = false;
}