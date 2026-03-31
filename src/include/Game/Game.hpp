#ifndef FE_GAME
#define FE_GAME

#include "include/Core/Scene.hpp"
#include "include/EngineController/EngineController.hpp"

class Game {
    private:
    shared_ptr<Scene> gameScene;
    shared_ptr<Camera> cam;

    EngineController engine;

    public:
    void Init();
    void Start();
};

#endif