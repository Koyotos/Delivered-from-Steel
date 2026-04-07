#ifndef FE_SPRITE
#define FE_SPRITE

#include "include/Renderer/Mesh.hpp"
#include <glad/glad.h>
#include <vector>
#include <filesystem>

using namespace std;
using namespace filesystem;

class Sprite {
    private:
    string directory;
    GLuint VAO, VBO, EBO;
    GLuint active;
    vec2 size;

    vector<Vertex2D> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    void SetupSprite();
    void LoadTextures();

    public:
    void SetActiveTexture(const GLuint&);
    void SetActiveTexture(const string&);

    vec2 GetSize();

    Texture& GetActiveTexture();
    const string& GetDir() const noexcept;
    void Draw(Shader&);

    Sprite(const string&);
    ~Sprite();
};

#endif