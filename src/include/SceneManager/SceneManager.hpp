#ifndef FE_SCENE_MANAGER
#define FE_SCENE_MANAGER

#include "include/Core/Scene.hpp"
#include <cstdint>
#include <vector>
#include <memory>

using namespace std;

class SceneManager {
    private:
    vector<shared_ptr<Scene>> scenes;
    uint8_t activeIndex;

    public:
    shared_ptr<Scene> GetActive();
};

#endif