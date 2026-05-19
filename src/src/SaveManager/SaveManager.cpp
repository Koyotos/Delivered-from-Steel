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

void SaveManager::LoadGame(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    nlohmann::json saveFile;
    file >> saveFile;
    file.close();

    if (saveFile.contains("active_scene")) {
        currentSceneToLoad = saveFile["active_scene"];
    }

    for (auto it = serializables.begin(); it != serializables.end(); ) {
        auto obj = it->second.lock();
        if (!obj) {
            it = serializables.erase(it);
            continue;
        }
        if (saveFile.contains(it->first)) {
            obj->Deserialize(saveFile[it->first]);
        }
        ++it;
    }
}

std::string SaveManager::GetCurrentSceneToLoad() const {
    return currentSceneToLoad;
}