#ifndef FE_PLATFORM
#define FE_PLATFORM

#include "include/Core/Object3D.hpp"
class Platform :
    public Object3D
{
    public:
		Platform(const unordered_map<string, std::any>&);
};

#endif // FE_PLATFORM
