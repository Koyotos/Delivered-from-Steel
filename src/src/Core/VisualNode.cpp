#include "include/Core/VisualNode.hpp"

shared_ptr<Shader> VisualNode::GetShader() {
    return shader;
}

Transform VisualNode::GetTransform() const noexcept {
    return transform;
}

void VisualNode::ApplyParentTransform(Transform t) {
    transform.Combine(t);
}

void VisualNode::ResetGlobal() {
    transform.ResetGlobal();
}

void VisualNode::SetShader(shared_ptr<Shader> sh) {
    shader = sh;
}   

void VisualNode::SetTransform(const Transform& transform) {
    this->transform = transform;
}