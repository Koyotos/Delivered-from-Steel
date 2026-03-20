#ifndef FE_RESOURCE_MANAGER
#define FE_RESOURCE_MANAGER

#include "include/Core/Scene.hpp"
#include "include/Renderer/Model.hpp"
#include "include/Renderer/Sprite.hpp"
#include <vector>
#include <memory>
#include <filesystem>

using namespace std;
using namespace filesystem;

struct RefCountModel {
    shared_ptr<Model> model = nullptr;
    uint8_t refCount = 0;
};

struct RefCountShader {
    shared_ptr<Shader> shader = nullptr;
    uint8_t refCount = 0;
};

struct RefCountSprite {
    shared_ptr<Sprite> sprite = nullptr;
    uint8_t refCount = 0;
};

class ResourceManager {
    private:
    path modelsPath;
    path spritesPath;
    path audioPath;
    path shadersPath;

    vector<RefCountModel> models;
    vector<RefCountSprite> sprites;
    vector<RefCountShader> shaders;
    vector<Scene> scenes;

    inline shared_ptr<Model> LoadModel();
    inline shared_ptr<Sprite> LoadSprite();
    inline shared_ptr<Shader> LoadShader();

    public:
    void LoadScene(const path&) noexcept;
};

#endif