#include "include/Core/Node.hpp"

vector<shared_ptr<Node>> Node::GetChildren() {
    return children;
}

void Node::AddChild(shared_ptr<Node> node) {
    children.push_back(node);
}

void Node::Process() {
    return;
}

bool Node::Input(InputEvent& event) {
    return false;
}

void Node::Draw(shared_ptr<Shader> sh) {
    return;
}

void Node::Physics(const float&) {
    return;
}

string Node::Type() {
    return "Node";
}

uint8_t Node::RenderType() {
    return 0;
}

void Node::Disable() noexcept {
    SetProcess(false);
    SetInput(false);
    SetPhysics(false);
    SetDraw(false);
    SetIgnoreParent(true);
    SetTransformChanged(false);
}

void Node::InitRecursive(shared_ptr<Scene> scene) {
    Init(scene);

    for (auto& child : GetChildren()) {
        child->InitRecursive(scene);
    }
}

Node::Node() {

}

Node::Node(const unordered_map<string, std::any>& data) {
    flags[0] = fromMap(bool, "process", data);
    flags[1] = fromMap(bool, "input", data);
    flags[2] = fromMap(bool, "physics", data);
}

Node::~Node() {
    
}