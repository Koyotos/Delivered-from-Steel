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

void Node::Draw() {
    return;
}

void Node::Physics(const float&) {
    return;
}

string Node::Type() {
    return "Node";
}

Node::Node() {

}

Node::Node(const unordered_map<string, any>& data) {
    flags[0] = fromMap(bool, "process", data);
    flags[1] = fromMap(bool, "input", data);
    flags[2] = fromMap(bool, "physics", data);
}

Node::~Node() {
    
}