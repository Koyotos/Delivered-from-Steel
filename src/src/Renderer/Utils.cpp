#include "include/Renderer/Utils.hpp"

Vertex::Vertex() {
    position = {0,0,0};
    normal = {0,0,0};
    uv = {0,0};
    tangent = {0,0,0};
    bitangent = {0,0,0};
    for(uint8_t i = 0; i < MAX_BONE_INFLUENCE; i++) {
        boneIDs[i] = -1;
        weights[i] = 0;
    }
}

Vertex::Vertex(const vec3& position, const vec3& normal, const vec2& uv, const vec3& tangent, const vec3& bitangent) {
    this->position = position;
    this->normal = normal;
    this->uv = uv;
    this->tangent = tangent;
    this->bitangent = bitangent;
    for(uint8_t i = 0; i < MAX_BONE_INFLUENCE; i++) {
        boneIDs[i] = -1;
        weights[i] = 0;
    }
}

mat4 GLMMat4(const aiMatrix4x4& from) {
    mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

vec3 GLMVec(const aiVector3D& vec) {
    return {vec.x,vec.y,vec.z};
};

vec2 GLMVec(const aiVector2D& vec) {
    return {vec.x,vec.y};
};