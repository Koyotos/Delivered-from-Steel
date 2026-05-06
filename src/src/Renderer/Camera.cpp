#include "include/Renderer/Camera.hpp"

mat4 Camera::GetVP(const bool& prspv) const noexcept {
    if(prspv) {
        return P*GetV();
    } else {
        return O*GetV();
    }
}

vec2 Camera::ToWorldCoords(const vec2& coords) const noexcept {
    return coords - position;
}

vec2 Camera::GetPos() {
    return position;
}

void Camera::SetPos(const vec2& pos) {
    position = pos;
}

Camera::Camera() {
    position = vec2(0);
}

Camera::Camera(const vec2& position) : Node() {
    this->position = position;
}

Camera::Camera(unordered_map<string,std::any> data) : Node(data) {
    vec2 jsonVec;
    jsonVec.x = fromMap(int64_t,"x",data);
    jsonVec.y = fromMap(int64_t,"y",data);
    this->position = jsonVec;
}

Camera::~Camera() {

}