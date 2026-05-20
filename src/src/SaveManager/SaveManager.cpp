#include <fstream>
#include <memory>
#include <algorithm>
#include "include/SaveManager/SaveManager.hpp"

void SaveManager::Register(std::shared_ptr<ISerializable> object) {
	if (object) {
		serializables[object->GetSerializeKey()] = object;
	}
}

void SaveManager::SaveGame(const std::string& filepath, const std::string& activeSceneName) {
	nlohmann::json saveFile;
	saveFile["active_scene"] = activeSceneName;

	for (auto it = serializables.begin(); it != serializables.end(); ) {
		auto obj = it->second.lock();
		if (!obj) {
			it = serializables.erase(it);
			continue;
		}
		saveFile[it->first] = obj->Serialize();
		++it;
	}

	std::ofstream file(filepath);
	if (file.is_open()) {
		file << saveFile.dump(4);
		file.close();
	}
}

bool SaveManager::LoadFile(const std::string& filepath) {
	loadedSaveFile.clear();
	currentSceneToLoad.clear();

	std::ifstream file(filepath);
	if (!file.is_open()) return false;

	try {
		file >> loadedSaveFile;
	}
	catch (...) {
		file.close();
		return false;
	}
	file.close();

	if (loadedSaveFile.contains("active_scene")) {
		currentSceneToLoad = loadedSaveFile["active_scene"].get<std::string>();
	}
	return true;
}

void SaveManager::ApplyLoaded() {
	if (loadedSaveFile.is_null()) return;

	for (auto it = serializables.begin(); it != serializables.end(); ) {
		auto obj = it->second.lock();
		if (!obj) {
			it = serializables.erase(it);
			continue;
		}
		if (loadedSaveFile.contains(it->first)) {
			obj->Deserialize(loadedSaveFile[it->first]);
		}
		++it;
	}

	loadedSaveFile.clear();
}

void SaveManager::LoadGame(const std::string& filepath) {
	if (!LoadFile(filepath)) return;
	ApplyLoaded();
}

std::string SaveManager::GetCurrentSceneToLoad() const {
	return currentSceneToLoad;
}