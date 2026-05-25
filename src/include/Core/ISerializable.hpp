#ifndef ISERIALIZABLE_HPP
#define ISERIALIZABLE_HPP

#include <nlohmann/json.hpp>
#include <string>

class ISerializable {
public:
	virtual ~ISerializable() = default;
	virtual std::string GetSerializeKey() const = 0;
	virtual nlohmann::json Serialize() const = 0;
	virtual void Deserialize(const nlohmann::json& data) = 0;
};

#endif