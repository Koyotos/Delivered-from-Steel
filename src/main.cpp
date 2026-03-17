#include "include/EngineController/EngineController.hpp"

int main(int argc, char* argv[]) {
    EngineController eng;
    eng.Init();
    eng.Run();
    return 0;
}