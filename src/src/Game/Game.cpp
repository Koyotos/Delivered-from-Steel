#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"

void Game::Init() {
    engine.Init();
    gameScene = engine.LoadScene("res/scenes/base.json");
    gameScene->GetRoot()->AddChild(engine.LoadScene("res/scenes/test3d.json")->GetRoot());
    gameScene->GetRoot()->AddChild(engine.LoadScene("res/scenes/test2d.json")->GetRoot());
    //gameScene->GetRoot()->AddChild(engine.LoadScene("res/scenes/testUI.json")->GetRoot());

    engine.SetActiveScene(gameScene);

    gameScene->GetRoot()->InitRecursive(gameScene);
}

void Game::Start() {
    engine.Run();
}