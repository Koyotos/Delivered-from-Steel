#ifndef FE_ENGINE_CONTROLLER
#define FE_ENGINE_CONTROLLER

#include <unordered_set>
#include "include/Globals/Globals.hpp"
#include "include/Renderer/Renderer.hpp"
#include "include/IOManager/IOManager.hpp"
#include "include/SceneManager/SceneManager.hpp"
#include "include/AudioManager/AudioManager.hpp"
#include "include/PhysicsManager/PhysicsManager.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/SaveManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"
#include "include/Game/UI/MenuManager.hpp"

/*
@brief Main engine class that acts as a facade for modules. It hides low-level mechanisms of modules
under API. Communication with engine should pass through this class.
*/
class EngineController {
    private:

    Globals* globals;
    PhysicsManager* psm;
    const char* confPath = "res/settings.json";
    
    shared_ptr<SceneManager> scm;
    shared_ptr<IOManager> iom;
    shared_ptr<Renderer> renderer;
    shared_ptr<ResourceManager> rsm;
    shared_ptr<AudioManager> aum;
	shared_ptr<CardManager> crm;
    shared_ptr<SaveManager> svm;
    shared_ptr<WorldStateManager> wsm;

    shared_ptr<Node> activeLevelNode = nullptr;
    shared_ptr<Node> previousLevelNode = nullptr;
    string activeLevelName = "";
    string previousLevelName = "";
    shared_ptr<MenuManager> mm;
    shared_ptr<Scene> menuScene;
    unordered_set<const Node*> registeredSerializableRoots;
	shared_ptr<Scene> activeScene = nullptr;
	shared_ptr<Scene> previousScene = nullptr;

    string pendingStreamLevel = "";
    bool pendingUnload = false;
    bool pendingSwap = false;
    bool pendingF9 = false;
    string pendingF9Path = "";
    bool pendingRespawn = false;
    bool discardAsyncResult = false;
    vector<shared_ptr<Node>> nodesToUnload;
	void FlattenForUnload(shared_ptr<Node> node);
	shared_ptr<Scene> nextLevelScene = nullptr;
	string nextLevelName = "";

    bool isAsyncLoading = false;
    string asyncLoadingName = "";

    double currentTime;
    double deltaTime; 
    double lastTime;

    inline void EndFrame();
    inline void ProcessNode(shared_ptr<Node>);
    void RegisterSceneSerializables(shared_ptr<Scene> scene);
    void RegisterSceneSerializables(shared_ptr<Node> root);
    void ApplyWorldStateToNode(shared_ptr<Node> root, const string& levelName);
    void ActivateLoadedScene(shared_ptr<Scene> loadedScene, const std::string& levelName);

    public:
    // Engine API
    
    /*
    @brief Intis engine and it's modules. This must be called before any other engine function.
    Otherwise engine will crash.
    @return void
    */
    void Init();


    /**
    @brief Resolves references and links objects inside the active scene.
    @return void
    */
    void LinkSceneObjects();

    /*
    @brief Starts the engine.
    @return void
    */
    void Run();

    /*
    @brief Loads scene from json file. Scene is loaded into engine and will be held there.
    Function returns pointer to scene for further usage. If loading fails this returns nullptr.
    @param1 const string& - scene path
    @return shared_ptr<Scene> - loaded scene
    */
    shared_ptr<Scene> LoadScene(const string&);

    /*
    @brief Sets active scene to a specified by pointer. If scene is not inside engine
    this call will fail.
    @param1 shared_ptr<Scene> - new active
    @return void
    */
    void SetActiveScene(shared_ptr<Scene>);

    /*
    @brief Sets active scene to a specified by index. If index is invalid this will fail without throw.
    this call will fail.
    @param1 const uint16_t& - new active index
    @return void
    */
    void SetActiveScene(const uint16_t&);

    void TransitionToMenu();

    shared_ptr<MenuManager> GetMenuManager() const;

    // Deletes of copy constructors
    EngineController(const EngineController&) = delete;
    EngineController(EngineController&&) = delete;

    /*
    @brief Basic constructor.
    @return EngineController
    */
    EngineController();

    /*
    @brief Basic destructor.
    */
    ~EngineController();

    void LoadLevel(const string& levelName);
    void UnloadPreviousLevel();
    void SwapActiveAndPrevious();
    void QueueStreamNextLevel(const std::string& levelName) { pendingStreamLevel = levelName; }
    void QueueUnloadPreviousLevel() { pendingUnload = true; }
    void QueueSwapActiveAndPrevious() { pendingSwap = true; }
    void TriggerRespawn();
    void CancelAsyncLoad();
    void ReadApplyConf();

    std::string GetActiveLevelName() const;
    std::string GetPreviousLevelName() const;

    void SaveGame(const string& filepath);
    void LoadGame(const string& filepath);
};

#endif