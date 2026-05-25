#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"

void Game::Init() {
    engine.Init();
    gameScene = engine.LoadScene("res/scenes/base.json");

    engine.TransitionToMenu();

    engine.GetMenuManager()->SetOnStartGame([this]() {
        engine.SetActiveScene(gameScene);
        engine.LinkSceneObjects();
        engine.LoadLevel("141_obiektow");
        });

    /* Jeœli chcesz bez menu do odkomentuj to i zakomentuj to u góry
     * 
     * engine.SetActiveScene(gameScene); 
     * engine.LinkSceneObjects();  
     * engine.LoadLevel("141_obiektow");
     */
     
}


void Game::Start() {
    engine.Run();
}