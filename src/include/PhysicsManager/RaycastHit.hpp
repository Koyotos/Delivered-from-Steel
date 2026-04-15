#pragma once

#include <glm/glm.hpp>
#include <memory>

class Collider;

struct RaycastHit
{
    glm::vec2 point;
    glm::vec2 normal;
    float distance;
    std::shared_ptr<Collider> collider;
};