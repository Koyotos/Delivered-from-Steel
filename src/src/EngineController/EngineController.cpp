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

	globals->engineController = this;
	globals->Log("Globals OK");

	try {
		renderer = make_shared<Renderer>();
		scm = make_shared<SceneManager>();
		iom = make_shared<IOManager>();
		rsm = make_shared<ResourceManager>();
		crm = make_shared<CardManager>();
		svm = make_shared<SaveManager>();
		wsm = make_shared<WorldStateManager>();
		aum = make_shared<AudioManager>();

		globals->ioManager = iom;
		globals->renderer = renderer;
		globals->cardManager = crm;
		globals->sceneManager = scm;
		globals->worldStateManager = wsm;

		if(!aum->Init()) {
			globals->Log("Audio failed to initialize. Game will continue without sound.");
			aum = nullptr;
		}
		rsm->SetAudioManager(aum);
		globals->audioManager = aum;

		rsm->ConfigurePaths();
		renderer->Init(*rsm);
		iom->Init(renderer->GetWindow());
		crm->Init(rsm);
		globals->SetGameFont(Font("res/fonts/8bit_wonder/8-BIT-WONDER.ttf",{32,32}));

		svm->Register(static_pointer_cast<ISerializable>(wsm));
		svm->Register(static_pointer_cast<ISerializable>(crm));

		ReadApplyConf();

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
		svm->Register(static_pointer_cast<ISerializable>(active->GetPlayer()));
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


void EngineController::ReadApplyConf() {
	string valueS;
	bool valueB;
	float valueF;
	
	unordered_map<string, std::any> confData = rsm->LoadJSON(confPath);

	// Window
	jsonVector size = fromMap(jsonVector, "windowSize", confData);
	renderer->Reconfigure(RCMD_RESIZE_W, any_cast<int64_t>(size[0]));
	renderer->Reconfigure(RCMD_RESIZE_H, any_cast<int64_t>(size[1]));
	renderer->Reconfigure(RCMD_REMAKE_WINDOW);

	valueB = fromMap(bool, "fullscreen", confData);
	renderer->Reconfigure(RCMD_FULLSCREEN);

	valueS = fromMap(string, "shadows", confData);
	renderer->Reconfigure(RCMD_SHADOW_QUALITY, valueS == "low" ?
		 RCMDVAL_SHADOWS_LOW : valueS == "medium" ? RCMDVAL_SHADOWS_MEDIUM : RCMDVAL_SHADOWS_HIGH);

	valueB = fromMap(bool, "godRays", confData);
	renderer->Reconfigure(RCMD_GOD_RAYS, valueB);

	valueB = fromMap(bool, "bloom", confData);
	renderer->Reconfigure(RCMD_BLOOM, valueB);

	valueF = fromMap(float, "cdpl", confData);
	renderer->Reconfigure(RCMD_POINT_CULL_DIST,0,valueF);

	valueF = fromMap(float, "cdsl", confData);
	renderer->Reconfigure(RCMD_SPOT_CULL_DIST,0,valueF);

	valueF = fromMap(float, "ddist", confData);
	renderer->Reconfigure(RCMD_DIR_DISTANCE,0,valueF);

	valueF = fromMap(float, "lcull", confData);
	renderer->Reconfigure(RCMD_LIGHT_CULL_RADIUS,0,valueF);
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

		globals->SetDeltaTime(static_cast<float>(deltaTime));

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
				globals->SetPhysicsTime(t);
			}
		}

		PROFILER_END_LOGIC();

		if (pendingSwap) {
			SwapActiveAndPrevious();
			pendingSwap = false;
		}
		if (pendingUnload) {
			UnloadPreviousLevel();
			pendingUnload = false;
		}
		if (!pendingStreamLevel.empty() && !isAsyncLoading) {
			std::filesystem::path fullPath = globals->GetExecDir() / "res" / "scenes" / (pendingStreamLevel + ".json");
			rsm->LoadSceneAsync(fullPath.string());
			isAsyncLoading = true;
			asyncLoadingName = pendingStreamLevel;
			pendingStreamLevel = "";
		}

		if (isAsyncLoading) {
			auto loadedScene = rsm->GetLoadedAsync(asyncLoadingName);

			if (loadedScene != nullptr) {
				if (pendingF9 || pendingRespawn || discardAsyncResult) {
					discardAsyncResult = false;
					FlattenForUnload(loadedScene->GetRoot());
					isAsyncLoading = false;
					asyncLoadingName = "";
				}
				else {
					nextLevelScene = loadedScene;
					nextLevelName = asyncLoadingName;

					rsm->AddLoadedScene(nextLevelScene);

					isAsyncLoading = false;
					asyncLoadingName = "";
					globals->Log("Pre-loaded scene: " + nextLevelName);
				}
			}
			else if (rsm->IsAsyncQueueEmpty()) {
				globals->Log("ERROR: Loading " + asyncLoadingName + " interrupted! Resetting flag.");
				isAsyncLoading = false;
				asyncLoadingName = "";
			}
		}

		if (!isAsyncLoading) {
			if (pendingF9) {
				pendingF9 = false;
				pendingRespawn = false;
				LoadGame(pendingF9Path);
				lastTime = glfwGetTime();
				accumulator = 0.0;
			}
			else if (pendingRespawn) {
				pendingRespawn = false;
				pendingF9 = false;
				TriggerRespawn();
			}
		}

		if (aum) {
			aum->Update();
		}

		iom->ClearQueue();

		renderer->DrawScene(active);

		PROFILER_END_RENDER();

		if (!isAsyncLoading && !nodesToUnload.empty()) {
			int nodesToDestroyThisFrame = 30;
			while (!nodesToUnload.empty() && nodesToDestroyThisFrame > 0) {
				nodesToUnload.pop_back();
				nodesToDestroyThisFrame--;
			}
		}

		EndFrame();

		//test save/load
		if (globals->wantsToSave) {
			SaveGame("save_0.json");
			globals->Log("TEST: Game Saved (F5)");
			globals->wantsToSave = false;
		}

		if (globals->wantsToLoad) {
			LoadGame("save_0.json");
			globals->Log("TEST: Game Loaded (F9)");
			globals->wantsToLoad = false;
		}
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

void EngineController::ActivateLoadedScene(shared_ptr<Scene> loadedScene, const std::string& levelName) {
	if (!loadedScene || !loadedScene->GetRoot()) {
		if (globals) globals->Log("ERROR: Loaded scene is null or has no root node.");
		return;
	}

	activeLevelName = levelName;
	globals->activeLevelName = activeLevelName;
	activeLevelNode = loadedScene->GetRoot();

	activeLevelNode->InitRecursive(scm->GetActive());
	scm->GetActive()->GetRoot()->AddChild(activeLevelNode);

	if (psm) {
		psm->RegisterNode(activeLevelNode);
		psm->Update(scm->GetActive(), 0.0f);
	}

	RegisterSceneSerializables(activeLevelNode);
	ApplyWorldStateToNode(activeLevelNode, activeLevelName);

	if (aum) {
		if (!loadedScene->scenePlaylist.empty()) {
			aum->PlayPlaylist(loadedScene->scenePlaylist);
		}
		//if (!loadedScene->sceneAmbient.empty()) {
		//	aum->PlayAmbient(loadedScene->sceneAmbient);
		//}
	}
}

void EngineController::RegisterSceneSerializables(shared_ptr<Scene> scene) {
	if (!scene || !scene->GetRoot()) return;

	RegisterSceneSerializables(scene->GetRoot());
}

void EngineController::RegisterSceneSerializables(shared_ptr<Node> root) {
	if (!root) return;

	if (!registeredSerializableRoots.insert(root.get()).second) {
		return;
	}

	auto registerSerializable = [&](auto& self, const shared_ptr<Node>& node) -> void {
		if (!node) return;

		auto serializable = dynamic_pointer_cast<ISerializable>(node);
		if (serializable) {
			svm->Register(serializable);
		}

		for (auto& child : node->GetChildren()) {
			self(self, child);
		}
		};

	registerSerializable(registerSerializable, root);
}

void EngineController::ApplyWorldStateToNode(shared_ptr<Node> root, const string& levelName) {
	if (!root || !wsm) return;

	auto disableAll = [&](auto& self, const shared_ptr<Node>& node) -> void {
		if (!node) return;

		node->Disable();
		for (auto& child : node->GetChildren()) {
			self(self, child);
		}
		};
	auto disableDestroyed = [&](auto& self,const shared_ptr<Node>& n) -> void {
		if (!n) return;

		auto id = n->GetSaveID();
		if (!id.empty() && wsm->IsDestroyed(levelName, id)) {
			disableAll(disableAll, n);
			return;
		}
		for (auto& child : n->GetChildren()) {
			self(self, child);
		}
		};
	disableDestroyed(disableDestroyed, root);
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
	if (aum) {
		if (!menuScene->scenePlaylist.empty()) {
			aum->PlayPlaylist(menuScene->scenePlaylist);
		}
		//if (!menuScene->sceneAmbient.empty()) {
		//	aum->PlayAmbient(menuScene->sceneAmbient);
		//}
	}
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
		globals->Log("Invalid level name.");
		return;
	}

	if (previousLevelNode) {
		UnloadPreviousLevel();
	}

	std::filesystem::path fullPath = globals->GetExecDir() / "res" / "scenes" / (levelName + ".json");

	if (activeLevelNode) {
		previousLevelNode = activeLevelNode;
		previousLevelName = activeLevelName;
		UnloadPreviousLevel();
		activeLevelNode = nullptr;
		activeLevelName = "";
	}

	shared_ptr<Scene> loadedLevel = rsm->LoadScene(fullPath);
	if (!loadedLevel) return;

	ActivateLoadedScene(loadedLevel, levelName);
}

void EngineController::UnloadPreviousLevel() {
	if (!previousLevelNode) return;

	registeredSerializableRoots.erase(previousLevelNode.get());

	if (psm) {
		psm->UnregisterNode(previousLevelNode);
	}

	auto activeScene = scm->GetActive();
	if (activeScene && activeScene->GetRoot()) {
		activeScene->GetRoot()->RemoveChild(previousLevelNode);
	}
	FlattenForUnload(previousLevelNode);
	previousLevelNode.reset();
	previousLevelName = "";
}

void EngineController::SwapActiveAndPrevious() {
	if (!nextLevelScene) return;

	if (activeLevelNode) {
		previousLevelNode = activeLevelNode;
		previousLevelName = activeLevelName;
	}
	
	ActivateLoadedScene(nextLevelScene, nextLevelName);

	nextLevelScene = nullptr;
	nextLevelName = "";
	pendingUnload = true;
}

void EngineController::CancelAsyncLoad() {
	if (isAsyncLoading) {
		discardAsyncResult = true;
	}
	pendingStreamLevel = "";
}

void EngineController::FlattenForUnload(shared_ptr<Node> node) {
	if (!node) return;
	for (auto& child : node->GetChildren()) {
		FlattenForUnload(child);
	}
	node->GetChildren().clear();
	nodesToUnload.push_back(node);
}

std::string EngineController::GetActiveLevelName() const {
	return activeLevelName;
}

std::string EngineController::GetPreviousLevelName() const {
	return previousLevelName;
}

void EngineController::SaveGame(const string& filepath) {
	auto saveDir = globals->GetExecDir() / "saves";
	std::filesystem::create_directories(saveDir);

	std::filesystem::path inputPath(filepath);
	if (inputPath.is_absolute() || inputPath.has_parent_path()) {
		globals->Log("Invalid save path.");
		return;
	}

	auto finalPath = (saveDir / inputPath).lexically_normal();

	shared_ptr<Scene> active = scm->GetActive();
	if (active && active->GetPlayer()) {
		svm->Register(std::static_pointer_cast<ISerializable>(active->GetPlayer()));
	}

	if (!svm->SaveGame(finalPath.string(), activeLevelName)) {
		globals->Log("Failed to save game.");
	}
}


void EngineController::LoadGame(const string& filepath) {
	auto saveDir = globals->GetExecDir() / "saves";

	std::filesystem::path inputPath(filepath);
	if (inputPath.is_absolute() || inputPath.has_parent_path()) {
		globals->Log("Invalid save path.");
		return;
	}

	auto finalPath = (saveDir / inputPath).lexically_normal();

	if (!svm->LoadFile(finalPath.string())) {
		globals->Log("Failed to open save file: " + finalPath.string());
		return;
	}

	string levelToLoad = svm->GetCurrentSceneToLoad();
	if (levelToLoad.empty()) {
		globals->Log("Save file missing active_scene.");
		return;
	}

	if (isAsyncLoading) {
		pendingF9 = true;
		pendingF9Path = filepath;
		return;
	}
	pendingStreamLevel = "";
	pendingRespawn = false;

	auto active = scm->GetActive();
	if (active) {
		if (auto player = active->GetPlayer()) {
			player->SuspendForLoading();
			player->SetVelocity(glm::vec2(0.0f));
			Transform t = player->GetTransform();
			t.SetTranslation(player->GetRespawnPoint());
			player->SetTransform(t);
		}
	}

	if (wsm) {
		wsm->ClearGraveyard();
	}

	LoadLevel(levelToLoad);

	active = scm->GetActive();

	svm->ApplyLoaded();
	ApplyWorldStateToNode(activeLevelNode, activeLevelName);

	if (psm) {
		psm->Update(active, 0.0f);
	}

	if (active) {
		if (auto player = active->GetPlayer()) {
			crm->AssignPlayer(player);
			player->Enable();
			player->Unsuspend();
		}
	}
}

void EngineController::TriggerRespawn() {
	auto active = scm->GetActive();
	if (!active || !active->GetPlayer()) return;

	auto player = active->GetPlayer();
	string respawnLevel = player->GetRespawnLevelName();

	if (activeLevelName != respawnLevel && !respawnLevel.empty()) {

		if (isAsyncLoading) {
			pendingRespawn = true;
			return;
		}
		pendingStreamLevel = "";

		player->SuspendForLoading();
		LoadLevel(respawnLevel);
		active = scm->GetActive();

		if (active && active->GetPlayer()) {
			player = active->GetPlayer();
			Transform t = player->GetTransform();
			t.SetTranslation(player->GetRespawnPoint());
			player->SetTransform(t);

			if (psm) {
				psm->Update(active, 0.0f);
			}

			player->Enable();
			player->Unsuspend();
		}
	}
	else {
		Transform t = player->GetTransform();
		t.SetTranslation(player->GetRespawnPoint());
		player->SetTransform(t);
		player->Enable();
	}
}