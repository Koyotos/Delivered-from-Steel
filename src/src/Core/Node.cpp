#include "include/Core/Node.hpp"

vector<shared_ptr<Node>> Node::GetChildren() {
    return children;
}

void Node::AddChild(shared_ptr<Node> node) {
    children.push_back(node);
}

void Node::RemoveChild(shared_ptr<Node> node) {
    auto it = find(children.begin(), children.end(), node);
    if (it != children.end()) {
        children.erase(it);
    }
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
    SetTransformChanged(true);
}

void Node::Enable() noexcept {
    SetProcess(true);
    SetInput(true);
    SetPhysics(true);
    SetDraw(true);
    SetTransformChanged(true);
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

    name.clear();
    auto it = data.find("name");
    if (it != data.end()) {
        if (const std::string* s = std::any_cast<std::string>(&it->second)) {
            name = *s;
        }
    }

    saveID.clear();
    auto itSave = data.find("saveID");
    if (itSave != data.end()) {
        if (const std::string* s = std::any_cast<std::string>(&itSave->second)) {
            saveID = *s;
        }
    }
}

Node::~Node() {
    
}