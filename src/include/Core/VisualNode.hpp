#ifndef FE_VISUAL_NODE
#define FE_VISUAL_NODE

#include "include/Core/Node.hpp"
#include "include/Core/Transform.hpp"
#include "include/Renderer/Model.hpp"
#include "include/Renderer/Shader.hpp"

/*
@brief Base class for visual nodes, it contains transform and methods neccessary for operating on it.
It is used as base class for it's 3D and 2D variants.
*/
class VisualNode : public Node {
    protected:
    Transform transform;
    shared_ptr<Shader> shader;
    float cullRadius;
    uint8_t zIndex;

    public:
    // Tests for engine purposes

    inline bool TestDraw() noexcept override final {return flags[3];};
    inline bool TestIgnoreParent() noexcept override final {return flags[4];};
    inline bool TestTransformChanged() noexcept override final {return flags[5];};

    string Type() override;
    uint8_t RenderType() override;

    /*
    @brief Sets z index. 
    @param1 const uint8_t& - z index
    @return void
    */
    void SetZIndex(const uint8_t&);

    /*
    @brief Returns z index. Z index is used only for text and objects 2d by renderer to group objects in layers.
    @return uint8_t - z index
    */
    uint8_t GetZIndex();

    /*
    @brief Returns object cull radius. Used only by renderer.
    @return float - radius
    */
    float GetCullRadius() const noexcept;

    /*
    @brief Sets Draw flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetDraw(const bool& state) noexcept override final {flags[3] = state;};

    /*
    @brief Sets IgnoreParent flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetIgnoreParent(const bool& state) noexcept override final {flags[4] = state;};

    /*
    @brief Sets TransformChanged flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetTransformChanged(const bool& state) noexcept override final {flags[5] = state;};

    /*
    @brief Returns shader used by node.
    @return shared_ptr<Shader> - shader
    */
    shared_ptr<Shader> GetShader();

    /*
    @brief Sets shader used by node.
    @param1 shared_ptr<Shader> - shader
    @return void
    */
    void SetShader(shared_ptr<Shader>);

    /*
    @brief Returns current node's transform.
    @return Transform - node's transform
    */
    Transform GetTransform() const noexcept;

    /*
    @brief Sets transform used by node and raises TransformChanged flag.
    @param1 const Transform& - new transform
    @return void
    */
    void SetTransform(const Transform&);

    /*
    @brief Function used by renderer to combine transforms in scene graph. Should not be used elsewhere.
    @param1 Transform - parent transform coming from upper node
    @return void
    */
    void ApplyParentTransform(Transform);

    /*
    @brief Resets global transform to be equal to local, effectively using object's local cooridinate system
    to draw it.
    @return void
    */
    void ResetGlobal();

    /*
    @brief Basic constructor. Creates empty VisualNode.
    @return VisualNode
    */
    VisualNode();

    /*
    @brief Override of Node's constructor. It calls parent class and inits own components.
    @param1 const unordered_map<string, std::any>& - data to init
    @return VisualNode
    */
    VisualNode(const unordered_map<string, std::any>&);
};

#endif