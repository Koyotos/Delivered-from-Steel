#include "include/EngineController/EngineController.hpp"
#include <functional>
#include <filesystem>

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
		crm = make_shared<CardManager>();
		svm = make_shared<SaveManager>();
		Globals::GetGlobals().sceneManager = scm;
		wsm = make_shared<WorldStateManager>();
		Globals::GetGlobals().worldStateManager = wsm;
		aum = make_shared<AudioManager>();
		if (!aum->Init()) {
			globals->Log("Audio failed to initialize. Game will continue without sound.");
			aum = nullptr;
		}
		rsm->SetAudioManager(aum);
		Globals::GetGlobals().audioManager = aum;

		rsm->ConfigurePaths();
		renderer->Init(*rsm);
		iom->Init(renderer->GetWindow());
		crm->Init(rsm);
		globals->SetGameFont(Font("res/fonts/verve/Verve.ttf",{0,50}));

		svm->Register(std::static_pointer_cast<ISerializable>(wsm));
		svm->Register(std::static_pointer_cast<ISerializable>(crm));

	} catch(const exception& except) {
		globals->Log("Engine initialization error : " + string(except.what()));
		exit(2);
	}
	PROFILER_INIT();
}

void EngineController::LinkSceneObjects() {
	shared_ptr<Scene> active = scm->GetActive();

	active->GetRoot()->InitRecursive(active);

	crm->AssignPlayer(active->GetPlayer());

	if (active->GetPlayer()) {
		svm->Register(std::static_pointer_cast<ISerializable>(active->GetPlayer()));
	}
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

		if (active && aum && active->GetPlayer()) {
			aum->SetListenerPosition(active->GetPlayer()->GetTransform().GetTranslation());
		}

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

		if (aum) {
			aum->Update();
		}

		iom->ClearQueue();

		renderer->DrawScene(active);

		PROFILER_END_RENDER();

		EndFrame();
		//test save/load
		if (Globals::GetGlobals().wantsToSave) {
			SaveGame("save_0.json");
			Globals::GetGlobals().Log("TEST: Game Saved (F5)");
			Globals::GetGlobals().wantsToSave = false;
		}

		if (Globals::GetGlobals().wantsToLoad) {
			LoadGame("save_0.json");
			Globals::GetGlobals().Log("TEST: Game Loaded (F9)");
			Globals::GetGlobals().wantsToLoad = false;
		}
	}
}

void EngineController::EndFrame() {
	renderer->EndFrame();
}

shared_ptr<Scene> EngineController::LoadScene(const string& name) {
	shared_ptr<Scene> scn = rsm->LoadScene(name);;
	scm->AddScene(scn);
	return scn;
}

void EngineController::RegisterSceneSerializables(shared_ptr<Scene> scene) {
	if (!scene || !scene->GetRoot()) return;

	std::function<void(shared_ptr<Node>)> registerSerializable = [&](shared_ptr<Node> node) {
		if (!node) return;

		auto serializable = std::dynamic_pointer_cast<ISerializable>(node);
		if (serializable) {
			svm->Register(serializable);
		}

		for (auto& child : node->GetChildren()) {
			registerSerializable(child);
		}
		};

	registerSerializable(scene->GetRoot());
}

void EngineController::SetActiveScene(shared_ptr<Scene> scn) {
	if (!scn) return;

	auto root = scn->GetRoot();
	if (!root) return;

	shared_ptr<Node> cardRoot = nullptr;
	if (crm && crm->GetCardScene()) {
		cardRoot = crm->GetCardScene()->GetRoot();
	}

	bool hasCards = false;
	bool hasCardRoot = false;

	for (auto& child : root->GetChildren()) {
		if (child == crm) {
			hasCards = true;
		}
		if (cardRoot && child == cardRoot) {
			hasCardRoot = true;
		}
		if (hasCards && hasCardRoot) {
			break;
		}
	}

	if (!hasCardRoot && cardRoot) {
		root->AddChild(cardRoot);
	}
	if (!hasCards && crm) {
		root->AddChild(crm);
	}

	scm->SetActive(scn);
}

void EngineController::SetActiveScene(const uint16_t& idx) {
	scm->SetActive(idx);
}

EngineController::EngineController() {

}

EngineController::~EngineController() {

}

void EngineController::LoadLevel(const string& levelName) {
	activeLevelName = levelName;
	Globals::GetGlobals().activeLevelName = levelName;

	std::filesystem::path fullPath = Globals::GetGlobals().GetExecDir() / "res" / "scenes" / (levelName + ".json");

	if (activeLevelNode) {
		scm->GetActive()->GetRoot()->RemoveChild(activeLevelNode);

		activeLevelNode.reset();
	}

	psm->Reset();

	shared_ptr<Scene> loadedLevel = rsm->LoadScene(fullPath);
	if (!loadedLevel) return;

	activeLevelNode = loadedLevel->GetRoot();
	activeLevelNode->InitRecursive(scm->GetActive());
	scm->GetActive()->GetRoot()->AddChild(activeLevelNode);
}


void EngineController::SaveGame(const string& filepath) {
	shared_ptr<Scene> active = scm->GetActive();
	if (active && active->GetPlayer()) {
		svm->Register(std::static_pointer_cast<ISerializable>(active->GetPlayer()));
	}

	svm->SaveGame(filepath, activeLevelName);
}


void EngineController::LoadGame(const string& filepath) {
	if (!svm->LoadFile(filepath)) {
		Globals::GetGlobals().Log("Failed to open save file: " + filepath);
		return;
	}

	string levelToLoad = svm->GetCurrentSceneToLoad();
	if (levelToLoad.empty()) return;

	LoadLevel(levelToLoad);

	auto active = scm->GetActive();
	RegisterSceneSerializables(active);

	if (scm->GetActive() && scm->GetActive()->GetPlayer()) {
		svm->Register(std::static_pointer_cast<ISerializable>(scm->GetActive()->GetPlayer()));
	}
	svm->Register(std::static_pointer_cast<ISerializable>(wsm));
	svm->Register(std::static_pointer_cast<ISerializable>(crm));

	svm->ApplyLoaded();

	if (scm->GetActive() && scm->GetActive()->GetPlayer()) {
		crm->AssignPlayer(scm->GetActive()->GetPlayer());
	}

	std::function<void(shared_ptr<Node>)> disableDestroyed = [&](shared_ptr<Node> node) {
		if (!node) return;

		string id = node->GetSaveID();
		if (!id.empty() && wsm->IsDestroyed(levelToLoad, id)) {
			node->Disable();
		}

		for (auto& child : node->GetChildren()) {
			disableDestroyed(child);
		}
		};

	if (activeLevelNode) {
		disableDestroyed(activeLevelNode);
	}
}