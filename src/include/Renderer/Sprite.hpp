#ifndef FE_SPRITE
#define FE_SPRITE

#include "include/Renderer/Mesh.hpp"
#include <glad/glad.h>
#include <vector>
#include <filesystem>
#include <string>
#include <unordered_map>

using namespace std;
using namespace filesystem;

struct AnimationData {
    string name;
    vector<GLuint> frames;
};

class Sprite {
    private:
    string directory;
    GLuint VAO, VBO, EBO;
    GLuint active;
    vec2 size;

    vector<Texture> textures;
    unordered_map<string, AnimationData> animations;

    void SetupSprite();
    void LoadTextures();

    public:
    void SetActiveTexture(const GLuint&);
    void SetActiveTexture(const string&);

    vec2 GetSize();

    Texture& GetActiveTexture();
    const string& GetDir() const noexcept;
    void Draw(Shader&);

    const AnimationData* GetAnimation(const string& name) const;
    bool HasAnimation(const string& name) const;

    Sprite(const string&);
    ~Sprite();
};

#endif