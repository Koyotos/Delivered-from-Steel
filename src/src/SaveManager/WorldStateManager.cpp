#include "include/SaveManager/WorldStateManager.hpp"

void WorldStateManager::MarkAsDestroyed(const std::string& sceneName, const std::string& objectID) {
	graveyard[sceneName].push_back(objectID);
}

bool WorldStateManager::IsDestroyed(const std::string& sceneName, const std::string& objectID) const {
	auto it = graveyard.find(sceneName);
	if (it != graveyard.end()) {
		for (const auto& id : it->second) {
			if (id == objectID) {
				return true;
			}
		}
	}
	return false;
}

void WorldStateManager::ClearGraveyard() {
	graveyard.clear();
}

std::string WorldStateManager::GetSerializeKey() const {
	return "world_state";
}

nlohmann::json WorldStateManager::Serialize() const {
	return graveyard;
}

void WorldStateManager::Deserialize(const nlohmann::json& data) {
	graveyard = data.get<std::unordered_map<std::string, std::vector<std::string>>>();
}