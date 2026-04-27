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

uint8_t VisualNode::RenderType() {
    return 1;
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

void VisualNode::SetZIndex(const uint8_t& z) {
    zIndex = z;
}

uint8_t VisualNode::GetZIndex() {
    return zIndex;
}

VisualNode::VisualNode() {

}

VisualNode::VisualNode(const unordered_map<string, std::any>& data) : Node(data) {
    transform = Transform(fromMap(vector<std::any>, "transform", data));
    flags[3] = fromMap(bool, "draw", data);
    flags[4] = fromMap(bool, "ignoreParent", data);
    //cullRadius = fromMap(float,"cull",data);
    //zIndex = (uint8_t)(fromMap(int64_t,"zIndex",data));
}