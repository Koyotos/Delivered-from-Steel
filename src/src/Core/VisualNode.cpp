#include "include/Core/VisualNode.hpp"

shared_ptr<Shader> VisualNode::GetShader() {
    return shader;
}

shared_ptr<Model> VisualNode::GetModel() {
    return model;
}

Transform VisualNode::GetTransform() const noexcept {
    return transform;
}

void VisualNode::ApplyParentTransform(Transform t) {
    transform.Combine(t);
}

void VisualNode::ResetGlobal() {
    
}

void VisualNode::SetModel(shared_ptr<Model> mdl) {
    model = mdl;
}

void VisualNode::SetShader(shared_ptr<Shader> sh) {
    shader = sh;
}   

void VisualNode::SetTransform(const Transform& transform) {
    this->transform = transform;
}

void VisualNode::Draw() {
    shader->SetMat4("M", transform.GetGlobal());
    model->Draw(*shader);
}