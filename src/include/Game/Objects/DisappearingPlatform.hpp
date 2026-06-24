#ifndef FE_DISAPPEARING_PLATFORM
#define FE_DISAPPEARING_PLATFORM

#include "include/Game/Objects/Platform.hpp"

class DisappearingPlatform : public Platform {
public:
    DisappearingPlatform(const std::unordered_map<std::string, std::any>& data);
    void Physics(const float& deltaTime) override;

private:
    float detectOffsetX = 0.0f;
    float detectArea = 0.5f;
};

#endif