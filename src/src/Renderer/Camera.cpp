#include "include/Renderer/Camera.hpp"

mat4 Camera::GetVP() const noexcept {
    mat4 V = translate(mat4(1.0), -vec3(position.x,position.y,0));
    return P*V;
}