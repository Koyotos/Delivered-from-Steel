#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"

void Game::Init() {
    engine.Init();

    
    engine.TransitionToMenu();
     

     // engine.SetActiveScene(gameScene); 
     // engine.LinkSceneObjects();  
	 // engine.LoadLevel("testLevel");
	 
     
     

}


void Game::Start() {
    engine.Run();
}