#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/matrix.hpp>
#include <glad/glad.h>
#include <string>

using namespace glm;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;

    Vertex();
    Vertex(const vec3&, const vec3&, const vec2&);
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

inline vec3 GLMVec(const aiVector3D& vec);
inline vec2 GLMVec(const aiVector2D& vec);