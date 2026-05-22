#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include "include/SaveManager/SaveManager.hpp"

namespace {
	constexpr int kSaveVersion = 1;
}

void SaveManager::Register(std::shared_ptr<ISerializable> object) {
	if (object) {
		serializables[object->GetSerializeKey()] = object;
	}
}

bool SaveManager::SaveGame(const std::string& filepath, const std::string& activeSceneName) {
	nlohmann::json saveFile;

	try {
		saveFile["version"] = kSaveVersion;
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
	} catch (...) {
		return false;
	}

	std::filesystem::path target = filepath;
	std::filesystem::path temp = target;
	temp += ".tmp";

	try {
		std::ofstream file(temp, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file << saveFile.dump(4);
	} catch (...) {
		std::error_code ec;
		std::filesystem::remove(temp, ec);
		return false;
	}

	std::error_code ec;
	std::filesystem::remove(target, ec);
	std::filesystem::rename(temp, target, ec);
	if (ec) {
		std::filesystem::remove(temp, ec);
		return false;
	}

	return true;
}

bool SaveManager::LoadFile(const std::string& filepath) {
	loadedSaveFile.clear();
	currentSceneToLoad.clear();

	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open()) return false;

	try {
		file >> loadedSaveFile;
	} catch (...) {
		return false;
	}

	if (!loadedSaveFile.is_object()) return false;

	int version = 0;
	if (loadedSaveFile.contains("version") && loadedSaveFile["version"].is_number_integer()) {
		version = loadedSaveFile["version"].get<int>();
	}

	if (version > kSaveVersion) {
		return false;
	}

	if (loadedSaveFile.contains("active_scene") && loadedSaveFile["active_scene"].is_string()) {
		currentSceneToLoad = loadedSaveFile["active_scene"].get<std::string>();
	}
	return true;
}

void SaveManager::ApplyLoaded() {
	if (!loadedSaveFile.is_object()) return;

	for (auto it = serializables.begin(); it != serializables.end(); ) {
		auto obj = it->second.lock();
		if (!obj) {
			it = serializables.erase(it);
			continue;
		}
		if (loadedSaveFile.contains(it->first)) {
			try {
				obj->Deserialize(loadedSaveFile[it->first]);
			} catch (...) {
				// skip corrupt record
			}
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