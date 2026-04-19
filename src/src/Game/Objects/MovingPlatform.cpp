#include "include/Game/Objects/MovingPlatform.hpp"

MovingPlatform::MovingPlatform(const unordered_map<string, std::any>& data) : Platform(data) {
	state = MovingPlatformState::StopStart;
	MovingDuration = fromMap(float, "movingDuration", data);
	StopDuration = fromMap(float, "stopDuration", data);
	startPosition = GetTransform().GetTranslation();
	endPosition = vec3( fromMap(float, "XendPosition", data), fromMap(float, "YendPosition",data), startPosition.z) ;
	timer = 0.0f;
}

vec3 Lerp(const vec3& a, const vec3& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return a + (b - a) * t;
}   

void MovingPlatform::Update(float deltaTime) {
    timer += deltaTime;

    vec3 from = (state == MovingPlatformState::MovingToEnd) ? startPosition : endPosition;
    vec3 to = (state == MovingPlatformState::MovingToEnd) ? endPosition : startPosition;
    float moveDuration = (state == MovingPlatformState::MovingToEnd) ? MovingDuration : StopDuration;
    float pauseDuration = (state == MovingPlatformState::StopEnd) ? MovingDuration : StopDuration;

    switch (state)
    {
    case MovingPlatformState::StopStart:
    case MovingPlatformState::StopEnd:
        if (timer >= pauseDuration) {
            state = (state == MovingPlatformState::StopEnd) ? MovingPlatformState::MovingToStart : MovingPlatformState::MovingToEnd;
            timer = 0;
        }
        break;

    case MovingPlatformState::MovingToEnd:
    case MovingPlatformState::MovingToStart:
    {
        float t = timer / moveDuration;

        Transform trans = GetTransform();

        vec3 pos = trans.GetTranslation();

        trans.SetTranslation(Lerp(from, to, t));

        SetTransform(trans);
        ResetGlobal();

        if (timer >= moveDuration) {
            state = (state == MovingPlatformState::MovingToStart) ? MovingPlatformState::StopStart : MovingPlatformState::StopEnd;
            timer = 0;
        }
        break;
    }
    }
}
