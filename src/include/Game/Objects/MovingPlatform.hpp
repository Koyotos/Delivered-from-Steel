#ifndef FE_MOVING_PLATFORM
#define FE_MOVING_PLATFORM

#include "include/Game/Objects/Platform.hpp"

enum class MovingPlatformState {
    MovingToEnd,
    MovingToStart,
    StopEnd,
	StopStart
};

class MovingPlatform : public Platform
{
private:
    MovingPlatformState state;

    float MovingDuration;
    float StopDuration;

    vec3 startPosition;
    vec3 endPosition;

    float timer;

public:
    MovingPlatform(const unordered_map<string, std::any>&);

    void Update(float deltaTime) override;
};

#endif // FE_MOVING_PLATFORM