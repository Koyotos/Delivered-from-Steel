#include "include/Game/Objects/LevelGateway.hpp"
#include "include/Globals/Globals.hpp"
#include "include/EngineController/EngineController.hpp"

LevelGateway::LevelGateway() = default;

LevelGateway::LevelGateway(const unordered_map<string, std::any>& data) : PhysicsNode(data) {
	if (data.find("levelNegative") != data.end()) {
		levelNegative = fromMap(std::string, "levelNegative", data);
	}
	if (data.find("levelPositive") != data.end()) {
		levelPositive = fromMap(std::string, "levelPositive", data);
	}
	if (data.find("isVerticalAxis") != data.end()) {
		isVerticalAxis = fromMap(bool, "isVerticalAxis", data);
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

	string active = engine->GetActiveLevelName();
	string prev = engine->GetPreviousLevelName();

	if (active != levelNegative && prev != levelNegative) {
		engine->QueueStreamNextLevel(levelNegative);
	}
	else if (active != levelPositive && prev != levelPositive) {
		engine->QueueStreamNextLevel(levelPositive);
	}
}

void LevelGateway::OnCollisionExit(shared_ptr<Collider> other) {
	auto owner = other ? other->GetOwner() : nullptr;
	if (!owner || owner->GetObjectType() != ObjectType::Player) return;

	auto& globals = Globals::GetGlobals();
	auto engine = globals.engineController;
	if (!engine) return;

	float playerPos = 0.0f;
	float volumePos = 0.0f;

	if (isVerticalAxis == true) {
		playerPos = owner->GetTransform().GetTranslation().y;
		volumePos = this->GetTransform().GetTranslation().y;
	}
	else {
		playerPos = owner->GetTransform().GetTranslation().x;
		volumePos = this->GetTransform().GetTranslation().x;
	}

	string targetActive = "";
	if (playerPos > volumePos) {
		targetActive = levelPositive;
	}
	else {
		targetActive = levelNegative;
	}

	string active = engine->GetActiveLevelName();

	if (active != targetActive) {
		engine->QueueSwapActiveAndPrevious();
	}
	engine->QueueUnloadPreviousLevel();
	hasTriggered = false;
}