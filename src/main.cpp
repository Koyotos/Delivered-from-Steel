#include "include/EngineController/EngineController.hpp"
#include "include/Game/Game.hpp"

int main(int argc, char* argv[]) {
    Game game;
    game.Init();
    game.Start();
    return 0;
}