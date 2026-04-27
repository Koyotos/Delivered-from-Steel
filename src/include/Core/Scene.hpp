#ifndef FE_SCENE
#define FE_SCENE

#include "include/Core/Node.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Renderer/Camera.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Renderer/Model.hpp"
#include "include/Renderer/Shader.hpp"
#include <memory>

using namespace std;

// Forward declarations
class Renderer;
class PhysicsManager;
class IOManager;
class ResourceManager;

class Scene {
    friend Renderer;
    friend PhysicsManager;
    friend IOManager;
    friend ResourceManager;
    
    private:
    string name;
    shared_ptr<Node> root;
    shared_ptr<Camera> sceneCam;
    shared_ptr<Player> scenePlayer;

    vector<shared_ptr<Model>> sceneModels; // TODO optimizations with unloading
    vector<shared_ptr<Shader>> sceneShaders;

    inline void FindCam(shared_ptr<Node>) noexcept;
    inline void FindPlayer(shared_ptr<Node>) noexcept;

    public:
    
    /*
    @brief Sets scene root. Root will be used as startpoint for all 
    per graph calls.
    @param1 shared_ptr<Node> = new root
    @return void
    */
    void SetRoot(shared_ptr<Node>);

    shared_ptr<Node> GetRoot();

	shared_ptr<Player> GetPlayer() { return scenePlayer; }

	/*
    * @brief Updates the transformation of a node and its children.
    * @param node The node to update.
    * @param parent The parent's transformation.
    */
    void UpdateTransforms(shared_ptr<Node>, Transform);
};

#endif
