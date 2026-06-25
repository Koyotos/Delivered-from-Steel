#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"

void Game::Init() {
    engine.Init();

    
    engine.TransitionToMenu();
    
    
    // shared_ptr<Scene> gameScene = engine.LoadScene("res/scenes/base.json");
	// engine.SetActiveScene(gameScene); 
	// engine.LinkSceneObjects();  
	// engine.LoadLevel("testLevel");

     
     

}


void Game::Start() {
    engine.Run();
}