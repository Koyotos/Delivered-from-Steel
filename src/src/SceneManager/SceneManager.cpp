#include "include/SceneManager/SceneManager.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Core/Object3D.hpp"
#include "include/Core/Transform.hpp"
#include "include/Renderer/Sprite.hpp"
#include <fstream>

void SceneManager::AddScene(shared_ptr<Scene> scn) {
    scenes.push_back(scn);
} 

shared_ptr<Scene> SceneManager::GetActive() {
    return scenes[activeIndex];
}

void SceneManager::SetActive(shared_ptr<Scene> scn) {
    for(uint16_t i = 0; i < scenes.size(); i++) {
        if(scn == scenes[i]) {
            activeIndex = i;
            return;
        }
    }
}

void SceneManager::SetActive(const uint16_t& idx) {
    if(idx < scenes.size() && idx>=0) {
        activeIndex = idx;
    }
}