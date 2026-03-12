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
    inline bool TestDraw() noexcept override final;
    inline bool TestIgnoreParent() noexcept override final;
    inline bool TestTransformChanged() noexcept override final;
    inline void SetDraw(const bool&) noexcept override final;
    inline void SetIgnoreParent(const bool&) noexcept override final;

    shared_ptr<Shader> GetShader();
    shared_ptr<Model> GetModel();
    Transform GetTransform() const noexcept;

    void ApplyParentTransform(Transform);
    void ResetGlobal();

    void SetModel(shared_ptr<Model>);
    void SetShader(shared_ptr<Shader>);
};