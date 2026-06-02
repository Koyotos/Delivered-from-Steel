#ifndef FE_RESOURCE_MANAGER
#define FE_RESOURCE_MANAGER

#include "include/Core/Object2D.hpp"
#include "include/Core/Object3D.hpp"
#include "include/Core/Scene.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Game/Objects/Trap.hpp"
#include "include/Game/Objects/Item.hpp"
#include "include/Game/Objects/Enemy.hpp"
#include "include/Game/Objects/ChargingEnemy.hpp"
#include "include/Game/Objects/TurretEnemy.hpp"
#include "include/Game/Objects/ShieldTankEnemy.hpp"
#include "include/Game/Objects/Bullet.hpp"
#include "include/Game/Objects/Platform.hpp"
#include "include/Game/Objects/NPC.hpp"
#include "include/Game/Objects/Button.hpp"
#include "include/Game/Objects/BreakableWall.hpp"
#include "include/Game/Objects/SpikePlatform.hpp"
#include "include/Game/Objects/MovingPlatform.hpp"
#include "include/Game/Objects/Drone.hpp"
#include "include/Game/Objects/CardFind.hpp"
#include "include/Game/Objects/Checkpoint.hpp"
#include "include/Renderer/Camera.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Renderer/TextNode.hpp"
#include "include/Game/UI/CardSlot.hpp"
#include "include/Game/UI/Icon.hpp"
#include "include/Game/UI/CardUI.hpp"
#include "include/Game/UI/Vignette.hpp"
#include "include/Profiler/ProfilerNode.hpp"
#include "include/Renderer/ParticleSystemNode.hpp"
#include "include/Game/Objects/ParticleEmitterNode.hpp"
#include "include/Game/Objects/OrbitalParticleSystem.hpp"
#include "include/AudioManager/AudioManager.hpp"
#include "include/Game/UI/TextUI.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>


class AudioManager;

using namespace std;
using namespace filesystem;
using namespace nlohmann;

template<typename T> pair<string,function<shared_ptr<Node>(const unordered_map<string,std::any>&)>> 
    RegisterObjectType(const string& name) {
        return {name,[](const unordered_map<string,std::any>& j)->shared_ptr<Node> 
            {return make_shared<T>(j);}};
}

static const pair<string,function<shared_ptr<Node>(const unordered_map<string,std::any>&)>> objectClassTable[] = {
    RegisterObjectType<Node>("Node"),
    RegisterObjectType<VisualNode>("VisualNode"),
    RegisterObjectType<Object3D>("Object3D"),
    RegisterObjectType<Object2D>("Object2D"),
    RegisterObjectType<Camera>("Camera"),
    RegisterObjectType<Light>("Light"),
    RegisterObjectType<Player>("Player"),
    RegisterObjectType<TextNode>("TextNode"),
	RegisterObjectType<CardSlot>("CardSlot"),
	RegisterObjectType<Icon>("Icon"),
	RegisterObjectType<Card>("Card"),
	RegisterObjectType<CardUI>("CardUI"),
	RegisterObjectType<Vignette>("Vignette"),
    RegisterObjectType<Trap>("Trap"),
    RegisterObjectType<Enemy>("Enemy"),
    RegisterObjectType<Drone>("Drone"),
	RegisterObjectType<CardFind>("CardFind"),
	RegisterObjectType<Checkpoint>("Checkpoint"),
    RegisterObjectType<ShieldTankEnemy>("ShieldTankEnemy"),
    RegisterObjectType<ChargingEnemy>("ChargingEnemy"),
    RegisterObjectType<TurretEnemy>("TurretEnemy"),
    RegisterObjectType<Bullet>("Bullet"),
    RegisterObjectType<Platform>("Platform"),
    RegisterObjectType<Item>("Item"),
    RegisterObjectType<NPC>("NPC"),
    RegisterObjectType<Button>("Button"),
    RegisterObjectType<BreakableWall>("BreakableWall"),
    RegisterObjectType<SpikePlatform>("SpikePlatform"),
	RegisterObjectType<MovingPlatform>("MovingPlatform"),
    RegisterObjectType<TextNode>("TextNode"),
    RegisterObjectType<ProfilerNode>("ProfilerNode"),
    RegisterObjectType<ParticleSystemNode>("ParticleSystemNode"),
    RegisterObjectType<ParticleEmitterNode>("ParticleEmitterNode"),
    RegisterObjectType<OrbitalParticleSystem>("OrbitalParticleSystem"),
    RegisterObjectType<TextUI>("TextUI")
};

struct RefCountModel {
    shared_ptr<Model> model = nullptr;
    uint8_t refCount = 0;
};

struct RefCountShader {
    shared_ptr<Shader> shader = nullptr;
    uint8_t refCount = 0;
};

struct RefCountSprite {
    shared_ptr<Sprite> sprite = nullptr;
    uint8_t refCount = 0;
};

class ResourceManager {
    private:
    path modelsPath;
    path spritesPath;
    path audioPath;
    path shadersPath;

    fstream resourceStream;

    vector<RefCountModel> models;
    vector<RefCountSprite> sprites;
    vector<RefCountShader> shaders;
    vector<shared_ptr<Scene>> scenes;

    shared_ptr<Scene> currentlyLoading;

    // Asset loaders
    inline shared_ptr<Model> LoadModel(const string&);
    inline shared_ptr<Sprite> LoadSprite(const string&);

    // Helper
    inline string LoadPlainText(const path&);

    // Json methods
    inline std::any JSONtoAny(const json&);
    inline unordered_map<string, std::any> LoadJSON(const path&);

    // Node parser helpers
    inline void ApplyAssetsGFX(shared_ptr<Node>, unordered_map<string,std::any>);
    inline void ApplyAssetsSFX(shared_ptr<Node>, unordered_map<string,std::any>);
    inline void ManageAudio(unordered_map<string,std::any>);
    inline vector<tuple<shared_ptr<Node>, int64_t, int64_t>> ParseNodes(unordered_map<string, std::any>&);
    inline void LinkScene(vector<tuple<shared_ptr<Node>, int64_t, int64_t>>&, shared_ptr<Scene>);

    shared_ptr<AudioManager> audioManager;

    public:
    shared_ptr<Scene> LoadScene(const path&) noexcept;
    void UnloadScene(shared_ptr<Scene>);
    void ConfigurePaths();

    void SetAudioManager(shared_ptr<AudioManager> aum) { audioManager = aum; }
    shared_ptr<Shader> LoadShader(const string&);

    ResourceManager();
    ~ResourceManager();
};

#endif