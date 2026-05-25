#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP

#include "include/Core/ISerializable.hpp"
#include "include/Globals/Globals.hpp"
#include <unordered_map>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;
using namespace filesystem;

class SaveManager {
	private:
	unordered_map<string, weak_ptr<ISerializable>> serializables;
	string currentSceneToLoad;
	json loadedSaveFile;

	public:
	void Register(shared_ptr<ISerializable> object);
	bool SaveGame(const string& filepath, const string& activeSceneName);
	bool LoadFile(const string& filepath);
	void ApplyLoaded();
	void LoadGame(const string& filepath);
	string GetCurrentSceneToLoad() const;
};

#endif