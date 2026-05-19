#ifndef WORLDSTATEMANAGER_HPP
#define WORLDSTATEMANAGER_HPP

#include "include/Core/ISerializable.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class WorldStateManager : public ISerializable {
private:
	std::unordered_map<std::string, std::vector<std::string>> graveyard;

public:
	void MarkAsDestroyed(const std::string& sceneName, const std::string& objectID);
	bool IsDestroyed(const std::string& sceneName, const std::string& objectID) const;
	void ClearGraveyard();

	std::string GetSerializeKey() const override;
	nlohmann::json Serialize() const override;
	void Deserialize(const nlohmann::json& data) override;
};

#endif