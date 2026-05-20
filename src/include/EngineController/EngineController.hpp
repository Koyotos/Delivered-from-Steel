#ifndef FE_ENGINE_CONTROLLER
#define FE_ENGINE_CONTROLLER

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

/*
@brief Main engine class that acts as a facade for modules. It hides low-level mechanisms of modules
under API. Communication with engine should pass through this class.
*/
class EngineController {
    private:

    Globals* globals;
    PhysicsManager* psm;

    shared_ptr<SceneManager> scm;
    shared_ptr<IOManager> iom;
    shared_ptr<Renderer> renderer;
    shared_ptr<ResourceManager> rsm;
    shared_ptr<AudioManager> aum;
	shared_ptr<CardManager> crm;
    shared_ptr<SaveManager> svm;
    shared_ptr<WorldStateManager> wsm;

    shared_ptr<Scene> activeLevelScene = nullptr;
    shared_ptr<Node> activeLevelNode = nullptr;
    std::string activeLevelName = "";

    double currentTime;
    double deltaTime; 
    double lastTime;

    inline void EndFrame();
    inline void ProcessNode(shared_ptr<Node>);

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

    void SaveGame(const string& filepath);
    void LoadGame(const string& filepath);
};

#endif