#ifndef FE_NODE
#define FE_NODE

#include <cstdint>
#include <memory>
#include <vector>

using namespace std;

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

    // Normal part again 

    /*
    @brief Returns vector of pointers to all node's children.
    @return vector<shared_ptr<Node>> - vector of children
    */
    vector<shared_ptr<Node>> GetChildren();

    /*
    
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
    virtual inline void SetTranformChanged(const bool& state) noexcept {return;};

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
    virtual bool Input();

    /*
    @brief Method that draws object's model. Called inside renderer.
    @return void
    */
    virtual void Draw();

    /*
    @brief Method that is responsible for physics related calculations. It's called inside PhysicsManager
    once every frame.
    @param1 const float& - time since last frame
    @return void
    */
    virtual void Physics(const float&);

    Node();
    virtual ~Node();

};

#endif