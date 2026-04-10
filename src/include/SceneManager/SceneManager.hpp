#ifndef FE_SCENE_MANAGER
#define FE_SCENE_MANAGER

#include "include/Core/Scene.hpp"
#include "include/Core/VisualNode.hpp"
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

    void SetActive(shared_ptr<Scene>);
    void SetActive(const uint16_t&);

    void AddScene(shared_ptr<Scene>);
};

#endif