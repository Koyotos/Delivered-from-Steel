#include "include/Renderer/Utils.hpp"

Vertex::Vertex() {
    position = {0,0,0};
    normal = {0,0,0};
    uv = {0,0};
}

Vertex::Vertex(const vec3& position, const vec3& normal, const vec2& uv) {
    this->position = position;
    this->normal = normal;
    this->uv = uv;
}

vec3 GLMVec(const aiVector3D& vec) {
    return {vec.x,vec.y,vec.z};
};

vec2 GLMVec(const aiVector2D& vec) {
    return {vec.x,vec.y};
};