#include "include/Renderer/Camera.hpp"

mat4 Camera::GetVP(const bool& prspv) const noexcept {
    mat4 V = translate(mat4(1.0), -vec3(position.x,position.y,0));
    if(prspv) {
        return P*V;
    } else {
        return O*V;
    }
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