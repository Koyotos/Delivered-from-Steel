#include "include/Core/VisualNode.hpp"

shared_ptr<Shader> VisualNode::GetShader() {
    return shader;
}

Transform VisualNode::GetTransform() const noexcept {
    return transform;
}

string VisualNode::Type() {
    return "VisualNode";
}

string VisualNode::RenderType() {
    return "VisualNode";
}

void VisualNode::ApplyParentTransform(Transform t) {
    transform.Combine(t);
    flags[5] = false;
}

void VisualNode::ResetGlobal() {
    transform.ResetGlobal();
}

void VisualNode::SetShader(shared_ptr<Shader> sh) {
    shader = sh;
}   

void VisualNode::SetTransform(const Transform& transform) {
    this->transform = transform;
	flags[5] = true;
}

float VisualNode::GetCullRadius() const noexcept {
    return cullRadius;
}

VisualNode::VisualNode() {

}

VisualNode::VisualNode(const unordered_map<string, std::any>& data) : Node(data) {
    transform = Transform(fromMap(vector<std::any>, "transform", data));
    flags[3] = fromMap(bool, "draw", data);
    flags[4] = fromMap(bool, "ignoreParent", data);
    cullRadius = fromMap(float,"cull",data);
}