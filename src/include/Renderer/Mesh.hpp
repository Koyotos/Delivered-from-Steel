#ifndef FE_MESH
#define FE_MESH

#include "Utils.hpp"
#include "include/Renderer/Shader.hpp"
#include "include/Profiler/Profiler.hpp"

#include <vector>

using namespace std;

class Mesh {
    private:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    GLuint VAO, VBO, EBO;
    GLuint transformBuffer;

    const size_t vec4size = sizeof(vec4);

    void SetupMesh(const bool&);

    public:

    void Draw(Shader&);
    void DrawInstanced(Shader&, vector<mat4>&);

    Mesh(vector<Vertex>, vector<GLuint>, vector<Texture>, const bool&);
    ~Mesh();
};

#endif