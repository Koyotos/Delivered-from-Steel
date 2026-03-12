#include "include/EngineController/EngineController.hpp"
#include "include/Globals/Globals.hpp"
#include <stdexcept>

void EngineController::Init() {

    try {
        globals = &Globals::GetGlobals();
    } catch(const exception& except) {
        exit(1);
    }

    globals->Log("Globals OK");

    try {
        scm = make_shared<SceneManager>();
        iom = make_shared<IOManager>();
        psm = make_shared<PhysicsManager>();
        renderer = make_shared<Renderer>();
        rsm = make_shared<ResourceManager>();
        aum = make_shared<AudioManager>();
    } catch(const exception& except) {
        globals->Log("Engine initialization error : " + string(except.what()));
        exit(2);
    }
}

void EngineController::Run() {
    while(1) {

    }
}

EngineController::EngineController() {

}

EngineController::~EngineController() {
    
}