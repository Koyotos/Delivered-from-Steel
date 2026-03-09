#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glad/glad.h>
#include <string>

using namespace glm;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};