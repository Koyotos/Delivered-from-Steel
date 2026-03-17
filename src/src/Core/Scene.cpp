#include "include/Core/Scene.hpp"

void Scene::SetRoot(shared_ptr<Node> root) {
    this->root = root;
}