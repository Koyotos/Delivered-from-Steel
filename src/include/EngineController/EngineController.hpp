#ifndef FE_ENGINE_CONTROLLER
#define FE_ENGINE_CONTROLLER

#include "include/Globals/Globals.hpp"
#include "include/Renderer/Renderer.hpp"
#include "include/IOManager/IOManager.hpp"
#include "include/SceneManager/SceneManager.hpp"
#include "include/AudioManager/AudioManager.hpp"
#include "include/PhysicsManager/PhysicsManager.hpp"
#include "include/ResourceManager/ResourceManager.hpp"

class EngineController {
    private:

    Globals* globals;

    shared_ptr<SceneManager> scm;
    shared_ptr<IOManager> iom;
    shared_ptr<PhysicsManager> psm;
    shared_ptr<Renderer> renderer;
    shared_ptr<ResourceManager> rsm;
    shared_ptr<AudioManager> aum;

    double currentTime;
    double deltaTime; 
    double lastTime;

    inline void EndFrame();

    public:
    // Engine API
    
    void Init();
    void Run();

    shared_ptr<Scene> LoadScene(const string&);
    void SetActiveScene(shared_ptr<Scene>);
    void SetActiveScene(const uint16_t&);

    EngineController(const EngineController&) = delete;
    EngineController(EngineController&&) = delete;

    EngineController();
    ~EngineController();
};

#endif