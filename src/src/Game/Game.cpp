#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"

void Game::Init() {
    engine.Init();
    gameScene = engine.LoadScene("res/scenes/base.json");

    /* 
    engine.TransitionToMenu();


    engine.GetMenuManager()->SetOnStartGame([this]() {
        engine.SetActiveScene(gameScene);
        engine.LinkSceneObjects();
        engine.LoadLevel("testLevel");
        });
    */
    
     
     engine.SetActiveScene(gameScene); 
     engine.LinkSceneObjects();  
	 engine.LoadLevel("jan_background");
     
     

}


void Game::Start() {
    engine.Run();
}