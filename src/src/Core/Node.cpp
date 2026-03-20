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

bool Node::Input() {
    return false;
}

void Node::Draw() {
    return;
}

void Node::Physics(const float&) {
    return;
}

Node::Node() {

}

Node::~Node() {
    
}