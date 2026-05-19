#ifndef FE_MOVING_PLATFORM
#define FE_MOVING_PLATFORM

#include "include/Game/Objects/Platform.hpp"

class Player;

enum class MovingPlatformState {
    MovingToEnd,
    MovingToStart,
    StopEnd,
	StopStart
};

class MovingPlatform : public Platform
{
private:
    vec3 lastPosition;

    MovingPlatformState state;

    float MovingDuration;
    float StopDuration;

    vec3 startPosition;
    vec3 endPosition;

    weak_ptr<Player> player;

    float timer;

public:
    vec3 velocityDelta;
    vec3 velocity;
    MovingPlatform(const unordered_map<string, std::any>&);

    void Physics(const float& deltaTime) override;

    void OnCollisionStay(shared_ptr<Collider> other) override;

    void OnCollisionExit(shared_ptr<Collider> other) override;

    void Init(shared_ptr<Scene>) override;
};

#endif // FE_MOVING_PLATFORM