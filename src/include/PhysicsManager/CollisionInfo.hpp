#ifndef FE_PHYSICS_COLLISION_INFO
#define FE_PHYSICS_COLLISION_INFO

#include <glm/vec2.hpp>
#include <memory>
class Collider;
#pragma once

struct CollisionInfo {
    bool collided = false;
    glm::vec2 normal = { 0, 0 };
    float depth = 0.0f;
    Collider* collider;
    CollisionInfo() = default;
};

#endif