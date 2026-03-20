#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Globals/Globals.hpp"
#include <filesystem>

shared_ptr<Model> ResourceManager::LoadModel() {

}

shared_ptr<Sprite> ResourceManager::LoadSprite() {

}

shared_ptr<Shader> ResourceManager::LoadShader() {

}

void ResourceManager::LoadScene(const path& scenePath) noexcept {
    if(!exists(scenePath)) {
        Globals::GetGlobals().Log("Non existend scene");
        return;
    }

    shared_ptr<Scene> loadedScene = make_shared<Scene>();

    for(const directory_entry& entry : directory_iterator(scenePath/path("models"))) {
        loadedScene->sceneModels.push_back(LoadModel());
    }

}