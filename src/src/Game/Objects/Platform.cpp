#include "include/Game/Objects/Platform.hpp"

Platform::Platform(const unordered_map<string, std::any>& data) : Object3D(data) {
	objectType = ObjectType::Wall;
}