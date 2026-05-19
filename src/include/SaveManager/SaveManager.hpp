#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP

#include "include/Core/ISerializable.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class SaveManager {
private:
    std::unordered_map<std::string, std::weak_ptr<ISerializable>> serializables;
    std::string currentSceneToLoad;

public:
    void Register(std::shared_ptr<ISerializable> object);
    void SaveGame(const std::string& filepath, const std::string& activeSceneName);
    void LoadGame(const std::string& filepath);
    std::string GetCurrentSceneToLoad() const;
};

#endif