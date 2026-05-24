#include "include/SaveManager/SaveManager.hpp"
#include "include/Globals/Globals.hpp"

namespace {
	constexpr int kSaveVersion = 1;
}

void SaveManager::Register(shared_ptr<ISerializable> object) {
	if(object) {
		serializables[object->GetSerializeKey()] = object;
	}
}

bool SaveManager::SaveGame(const string& filepath, const string& activeSceneName) {
	json saveFile;
	try {
		saveFile["version"] = kSaveVersion;
		saveFile["active_scene"] = activeSceneName;

		for(auto it = serializables.begin(); it != serializables.end();) {
			auto obj = it->second.lock();
			if(!obj) {
				it = serializables.erase(it);
				continue;
			}
			saveFile[it->first] = obj->Serialize();
			it++;
		}
	} catch (const exception& except) {
		Globals::GetGlobals().Log("Save failed : " + string(except.what()));
		return false;
	}

	path target = filepath;
	path temp = target;
	temp += ".tmp";

	try {
		ofstream file(temp, ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file << saveFile.dump(4);
	} catch (const exception& except) {
		Globals::GetGlobals().Log("Save failed : " + string(except.what()));
		error_code ec;
		remove(temp, ec);
		return false;
	}

	error_code ec;
	remove(target, ec);
	rename(temp, target, ec);
	if (ec) {
		remove(temp, ec);
		return false;
	}

	return true;
}

bool SaveManager::LoadFile(const string& filepath) {
	loadedSaveFile.clear();
	currentSceneToLoad.clear();

	ifstream file(filepath, ios::binary);

	if(!file.is_open()) {
		Globals::GetGlobals().Log("Load failed : can't open save file");
		return false;
	}
	try {
		file >> loadedSaveFile;
	} catch (const exception& except) {
		Globals::GetGlobals().Log("Load failed : " + string(except.what()));
		return false;
	}

	if(!loadedSaveFile.is_object()) return false;

	int version = 0;
	if(loadedSaveFile.contains("version") && loadedSaveFile["version"].is_number_integer()) {
		version = loadedSaveFile["version"].get<int>();
	}

	if(version > kSaveVersion) {
		Globals::GetGlobals().Log("Load failed : save version mismatch");
		return false;
	}

	if(loadedSaveFile.contains("active_scene") && loadedSaveFile["active_scene"].is_string()) {
		currentSceneToLoad = loadedSaveFile["active_scene"].get<string>();
	}
	return true;
}

void SaveManager::ApplyLoaded() {
	if(!loadedSaveFile.is_object()) return;

	for(auto it = serializables.begin(); it != serializables.end(); ) {
		auto obj = it->second.lock();
		if(!obj) {
			it = serializables.erase(it);
			continue;
		}
		if(loadedSaveFile.contains(it->first)) {
			try {
				obj->Deserialize(loadedSaveFile[it->first]);
			} catch (const exception& except) {
				Globals::GetGlobals().Log("Corrupted record :" + string(except.what()));
			}
		}
		it++;
	}
	loadedSaveFile.clear();
}

void SaveManager::LoadGame(const string& filepath) {
	if (!LoadFile(filepath)) return;
	ApplyLoaded();
}

string SaveManager::GetCurrentSceneToLoad() const {
	return currentSceneToLoad;
}