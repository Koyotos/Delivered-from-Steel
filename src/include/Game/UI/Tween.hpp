#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

enum class EaseType {
    Linear,
    InSine,
    OutSine,
    InOutSine,
    InQuad,
    OutQuad,
    InOutQuad,
};


struct Tween {
    enum class Type { Move, Fade, Tint } type;

    float duration;
    float elapsed;
    EaseType ease;

    // Move
    glm::vec2 startPos;
    glm::vec2 targetPos;

    // Fade
    float startAlpha;
    float targetAlpha;

    // Tint
    glm::vec3 startTint;
    glm::vec3 targetTint;

    bool finished = false;
};