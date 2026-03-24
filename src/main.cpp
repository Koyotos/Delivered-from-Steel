#include "include/EngineController/EngineController.hpp"

int main(int argc, char* argv[]) {
    EngineController eng;
    eng.Init();
    eng.SetActiveScene(eng.LoadScene("res/scenes/test.json"));
    eng.Run();
    return 0;
}