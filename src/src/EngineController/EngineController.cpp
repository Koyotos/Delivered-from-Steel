#include "include/EngineController/EngineController.hpp"

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
        rsm = make_shared<ResourceManager>();
        renderer = make_shared<Renderer>();
        aum = make_shared<AudioManager>();

        rsm->ConfigurePaths();
        renderer->Init(*rsm);
		iom->Init(renderer->GetWindow());
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
	double t = 0.0;
	const double fixedDeltaTime = 1.0 / 60.0;
	lastTime = glfwGetTime();
	double accumulator = 0.0;
    while(!glfwWindowShouldClose(renderer->GetWindow())) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        if (deltaTime > 0.25) {
            deltaTime = 0.25;
		}

		lastTime = currentTime;
		accumulator += deltaTime;

		Globals::GetGlobals().SetDeltaTime(static_cast<float>(deltaTime));

        shared_ptr<Scene> active = scm->GetActive();

		PROFILER_BEGIN_FRAME(static_cast<float>(deltaTime));

		glfwPollEvents();
		iom->PollGamepad();
        if (active) {
		    iom->ProcessInput(active->GetRoot());
            ProcessNode(active->GetRoot());

            while (accumulator >= fixedDeltaTime) {
                psm->Update(active, static_cast<float>(fixedDeltaTime));
                accumulator -= fixedDeltaTime;
                t += fixedDeltaTime;
                Globals::GetGlobals().SetPhysicsTime(t);
			}
        }

		PROFILER_END_LOGIC();

		iom->ClearQueue();

        renderer->DrawScene(active);

		PROFILER_END_RENDER();

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