#ifndef FE_VISUAL_NODE
#define FE_VISUAL_NODE

#include "include/Core/Node.hpp"
#include "include/Core/Transform.hpp"
#include "include/Renderer/Model.hpp"
#include "include/Renderer/Shader.hpp"

class VisualNode : public Node {
    private:
    Transform transform;
    shared_ptr<Shader> shader;
    shared_ptr<Model> model;

    public:
    inline bool TestDraw() noexcept override final {return flags[3];};
    inline bool TestIgnoreParent() noexcept override final {return flags[4];};
    inline bool TestTransformChanged() noexcept override final {return flags[5];};
    inline void SetDraw(const bool& state) noexcept override final {flags[3] = state;};
    inline void SetIgnoreParent(const bool& state) noexcept override final {flags[4] = state;};

    void Draw() override final;

    shared_ptr<Shader> GetShader();
    shared_ptr<Model> GetModel();
    Transform GetTransform() const noexcept;
    void SetTransform(const Transform&);

    void ApplyParentTransform(Transform);
    void ResetGlobal();

    void SetModel(shared_ptr<Model>);
    void SetShader(shared_ptr<Shader>);
};

#endif