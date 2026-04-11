#include "include/Game/Objects/Item.hpp"

Item::Item(const unordered_map<string, std::any>& data) : Object3D(data) {
	objectType = ObjectType::Default;
}