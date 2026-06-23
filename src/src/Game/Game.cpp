#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"

void Game::Init() {
    engine.Init();


    engine.TransitionToCutscene("res/scenes/openingCutscene.json");

    /*
    engine.TransitionToMenu();
     
    engine.GetMenuManager()->SetOnStartGame([this]() {
        engine.SetActiveScene(gameScene);
        engine.LinkSceneObjects();
        engine.LoadLevel("testLevel");
        });
    */
    
     
     // engine.SetActiveScene(gameScene); 
     // engine.LinkSceneObjects();  
	 // engine.LoadLevel("testLevel");
	 
     
     

}


void Game::Start() {
    engine.Run();
}