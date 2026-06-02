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
		Globals::GetGlobals().ioManager = iom;
		rsm = make_shared<ResourceManager>();
		renderer = make_shared<Renderer>();
		crm = make_shared<CardManager>();
		Globals::GetGlobals().cardManager = crm;
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
		globals->SetGameFont(Font("res/fonts/8bit_wonder/8-BIT-WONDER.ttf",{32,32}));

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
	active->GetPlayer()->SetCardManager(crm);

	if (active->GetPlayer()) {
		svm->Register(std::static_pointer_cast<ISerializable>(active->GetPlayer()));
	}
	RegisterSceneSerializables(active);
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

		iom->UpdateVibration(static_cast<float>(deltaTime));

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

shared_ptr<MenuManager> EngineController::GetMenuManager() const {
	return mm;
}	


void EngineController::TransitionToMenu() {
	if (!mm) {
		mm = make_shared<MenuManager>();
		mm->Init(rsm);
		menuScene = mm->GetMenuScene();
		menuScene->GetRoot()->AddChild(mm);
		scm->AddScene(menuScene);
	}

	menuScene->GetRoot()->InitRecursive(menuScene);
	scm->SetActive(menuScene);   // raw swap, no crm/cards injected
}

void EngineController::SetActiveScene(const uint16_t& idx) {
	scm->SetActive(idx);
}

EngineController::EngineController() {

}

EngineController::~EngineController() {

}

void EngineController::LoadLevel(const string& levelName) {
	auto levelPath = std::filesystem::path(levelName);
	if (levelPath.is_absolute() || levelPath.has_parent_path()) {
		Globals::GetGlobals().Log("Invalid level name.");
		return;
	}

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
	auto saveDir = Globals::GetGlobals().GetExecDir() / "saves";
	std::filesystem::create_directories(saveDir);

	std::filesystem::path inputPath(filepath);
	if (inputPath.is_absolute() || inputPath.has_parent_path()) {
		Globals::GetGlobals().Log("Invalid save path.");
		return;
	}

	auto finalPath = (saveDir / inputPath).lexically_normal();

	shared_ptr<Scene> active = scm->GetActive();
	if (active && active->GetPlayer()) {
		svm->Register(std::static_pointer_cast<ISerializable>(active->GetPlayer()));
	}

	if (!svm->SaveGame(finalPath.string(), activeLevelName)) {
		Globals::GetGlobals().Log("Failed to save game.");
	}
}


void EngineController::LoadGame(const string& filepath) {
	auto saveDir = Globals::GetGlobals().GetExecDir() / "saves";

	std::filesystem::path inputPath(filepath);
	if (inputPath.is_absolute() || inputPath.has_parent_path()) {
		Globals::GetGlobals().Log("Invalid save path.");
		return;
	}

	auto finalPath = (saveDir / inputPath).lexically_normal();

	if (!svm->LoadFile(finalPath.string())) {
		Globals::GetGlobals().Log("Failed to open save file: " + finalPath.string());
		return;
	}

	string levelToLoad = svm->GetCurrentSceneToLoad();
	if (levelToLoad.empty()) {
		Globals::GetGlobals().Log("Save file missing active_scene.");
		return;
	}

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

	active->GetRoot()->InitRecursive(active);
}


