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

        renderer->Init();
        rsm->ConfigurePaths();

    } catch(const exception& except) {
        globals->Log("Engine initialization error : " + string(except.what()));
        exit(2);
    }
}

void EngineController::Run() {
    while(1) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        shared_ptr<Scene> active = scm->GetActive();

		psm->Update(active, deltaTime);

        renderer->DrawScene(active);

        lastTime = currentTime;
        EndFrame();
    }
}

void EngineController::EndFrame() {
    renderer->EndFrame();
}

shared_ptr<Scene> EngineController::LoadScene(const string& name) {
    shared_ptr<Scene> scn = rsm->LoadScene(name);
    scm->AddScene(scn);
    return scn;
}

void EngineController::SetActiveScene(shared_ptr<Scene> scn) {
    scm->SetActive(scn);
}

void EngineController::SetActiveScene(const uint16_t& idx) {
    scm->SetActive(idx);
}

EngineController::EngineController() {

}

EngineController::~EngineController() {
    
}