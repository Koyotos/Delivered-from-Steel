#ifndef FE_NODE
#define FE_NODE

#include <any>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include "include/IOManager/InputEvent.hpp"
#include "include/Renderer/Shader.hpp"
#include <stdexcept>

#define fromMap(type,key,data) FromMap<type>(data,key)

template<typename T>
T FromMap(const std::unordered_map<std::string, std::any>& data, const std::string& key) {
    auto it = data.find(key);

    if (it == data.end()) {
        throw std::runtime_error("Missing key: " + key);
    }

    return std::any_cast<T>(it->second);
}

using namespace std;

class Scene;

/*
@brief Base class for all in-game objects. Every object that is going to be used inside scene graphs
has to inherit from this class. This will also provide basic functionality like flags for scene graph and
children list.
*/
class Node {
    protected:
    bool flags[6];
    vector<shared_ptr<Node>> children;

    public:
    // Internal stuff to communication between nodes and EngineController. This part is not needed anywhere else. Don't look here. Go away. //
    const uint8_t& GetFlags() const noexcept;

    inline bool TestProcess() noexcept {return flags[0];} 
    inline bool TestInput() noexcept {return flags[1];}
    inline bool TestPhysics() noexcept {return flags[2];}
    virtual inline bool TestDraw() noexcept {return false;}
    virtual inline bool TestIgnoreParent() noexcept {return true;}
    virtual inline bool TestTransformChanged() noexcept {return false;};

    // debug purpose only
    inline void DrawDebug() { return;  }

    // Normal part again 

    /*
    @brief Returns vector of pointers to all node's children.
    @return vector<shared_ptr<Node>> - vector of children
    */
    vector<shared_ptr<Node>> GetChildren();

    /*
    @brief Adds new child to Node.
    @param1 shared_ptr<Node> - child
    @return void
    */
    void AddChild(shared_ptr<Node>);

    /*
    @brief Sets Process flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetProcess(const bool& state) noexcept {flags[0] = state;};

    /*
    @brief Sets Input flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetInput(const bool& state) noexcept {flags[1] = state;};

    /*
    @brief Sets Physics flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetPhysics(const bool& state) noexcept {flags[2] = state;};

    /*
    @brief Sets Draw flag state.
    @param1 const bool& - state to be set
    @return void
    */
    virtual inline void SetDraw(const bool& state) noexcept {return;};

    /*
    @brief Sets Ignore Parent flag state.
    @param1 const bool& - state to be set
    @return void
    */
    virtual inline void SetIgnoreParent(const bool& state) noexcept {return;};

    /*
    @brief Sets Transform Changed flag state.
    @param1 const bool& - state to be set
    @return void
    */
    virtual inline void SetTransformChanged(const bool& state) noexcept {return;};

    /*
    @brief General purpose method, called once every frame. This may contain code that is related
    to object in-game behaviour.
    @return void
    */
    virtual void Process();

    /*
    @brief Method that is called for every input event queued by engine. It should contain
    every reaction to input related stuff.
    @param1 const InputEvent& - event to test
    @return bool - was the event handled
    */
    virtual bool Input(InputEvent& event);

    /*
    @brief Method that draws object's model. Called inside renderer.
    @return void
    */
    virtual void Draw(shared_ptr<Shader> sh = nullptr);

    /*
    @brief Method that is responsible for physics related calculations. It's called inside PhysicsManager
    once every frame.
    @param1 const float& - time since last frame
    @return void
    */
    virtual void Physics(const float&);

    /*
    @brief Returns node type as string.
    @return string - type
    */
    virtual string Type();

    /*
    @brief Returns node type as string for renderer.
    @return string - type
    */
    virtual uint8_t RenderType();

    /**
    @brief Disables all active behaviours of the node.
    Simulates destruction without removing it from scene graph.
    */
    void Disable() noexcept;

    /**
    @brief Enables all previously disabled behaviours of the node.
    @return void
    */
    void Enable() noexcept;

	/*
    * @brief Initializes the node and all its children.
    * @return void
    */
    void InitRecursive(shared_ptr<Scene>);

	/*
    * @brief Initializes the node.
    * @return void
    */
    virtual void Init(shared_ptr<Scene>) {}

    /*
    @brief Basic constructor. Creates empty Node.
    @return Node
    */
    Node();

    /*
    @brief Constructor from json. Takes data read from scene files and sets Node's fields.
    @param1 const unordered_map<string, any>& - data from json
    @return Node
    */
    Node(const unordered_map<string, std::any>&);

    /*
    @brief Generic destructor.
    */
    virtual ~Node();

};

#endif