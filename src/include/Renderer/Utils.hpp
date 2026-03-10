#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/matrix.hpp>
#include <glad/glad.h>
#include <string>

using namespace glm;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;
    int boneIDs[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];

    Vertex();
    Vertex(const vec3&, const vec3&, const vec2&, const vec3&, const vec3&);
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

struct BoneInfo {
    int id;
    mat4 offset;
};

inline mat4 GLMMat4(const aiMatrix4x4&);
inline vec3 GLMVec(const aiVector3D& vec);
inline vec2 GLMVec(const aiVector2D& vec);