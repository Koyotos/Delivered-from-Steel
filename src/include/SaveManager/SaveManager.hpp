#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP

#include "include/Core/ISerializable.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

class SaveManager {
private:
	std::unordered_map<std::string, std::weak_ptr<ISerializable>> serializables;
	std::string currentSceneToLoad;
	nlohmann::json loadedSaveFile;

public:
	void Register(std::shared_ptr<ISerializable> object);
	bool SaveGame(const std::string& filepath, const std::string& activeSceneName);
	bool LoadFile(const std::string& filepath);
	void ApplyLoaded();
	void LoadGame(const std::string& filepath);
	std::string GetCurrentSceneToLoad() const;
};

#endif