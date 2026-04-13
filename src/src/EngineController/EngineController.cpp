#include "include/EngineController/EngineController.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Profiler/Profiler.hpp"
#include <stdexcept>

void EngineController::Init() {

    try {
        globals = &Globals::GetGlobals();
    } catch(const exception& except) {
        exit(1);
    }

    try {
        psm = &PhysicsManager::GetPhysicsManager();
    } catch (const exception& except) {
        exit(1);
    }

    globals->Log("Globals OK");

    try {
        scm = make_shared<SceneManager>();
        iom = make_shared<IOManager>();
        renderer = make_shared<Renderer>();
        rsm = make_shared<ResourceManager>();
        aum = make_shared<AudioManager>();

		iom->Init(renderer->GetWindow());
        rsm->ConfigurePaths();
        globals->SetGameFont(Font("res/fonts/verve/Verve.ttf",{0,50}));

    } catch(const exception& except) {
        globals->Log("Engine initialization error : " + string(except.what()));
        exit(2);
    }
    PROFILER_INIT();
}

void EngineController::ProcessNode(shared_ptr<Node> node) {
	if (!node) return;

    if (node->TestProcess()) {
        node->Process();
	}

    for (auto& child : node->GetChildren()) {
        ProcessNode(child);
	}
}

void EngineController::Run() {
	lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(renderer->GetWindow())) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        shared_ptr<Scene> active = scm->GetActive();

		PROFILER_BEGIN_FRAME(deltaTime);

		psm->Update(active, deltaTime);
		glfwPollEvents();
		iom->PollGamepad();
        if (active) {
		    iom->ProcessInput(active->GetRoot());
            ProcessNode(active->GetRoot());
        }

		PROFILER_END_LOGIC();

		iom->ClearQueue();

        renderer->DrawScene(active);

		PROFILER_END_RENDER();

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