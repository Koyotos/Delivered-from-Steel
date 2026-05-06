#include "include/Core/Scene.hpp"

void Scene::SetRoot(shared_ptr<Node> root) {
    this->root = root;
    FindCam(root);
    FindPlayer(root);
    if(scenePlayer && sceneCam) {
        scenePlayer->SetCamera(sceneCam);
    }
}

shared_ptr<Node> Scene::GetRoot() {
    return root;
}

void Scene::FindCam(shared_ptr<Node> node) noexcept {
    if(auto cast = dynamic_pointer_cast<Camera>(node)) {
        sceneCam = cast;
        return;
    }
    for(auto& k : node->GetChildren()) {
        FindCam(k);
    }
}   

void Scene::FindPlayer(shared_ptr<Node> node) noexcept {
    if(auto cast = dynamic_pointer_cast<Player>(node)) {
        scenePlayer = cast;
        return;
    }
    for(auto& k : node->GetChildren()) {
        FindPlayer(k);
    }
}

void Scene::UpdateTransforms(shared_ptr<Node> node, Transform t) {

    if(node->RenderType()>=3) {
        shared_ptr<PhysicsNode> physicsCast = static_pointer_cast<PhysicsNode>(node);
        if (!physicsCast->TestIgnoreParent()) {
            if (physicsCast->TestTransformChanged()) {
                physicsCast->ApplyParentTransform(t);
                physicsCast->SetTransformChanged(false);
            }
        }
        else {
            physicsCast->ResetGlobal();
        }
        t = physicsCast->GetTransform();
    }


    for (const auto& child : node->GetChildren()) {
        Transform childTransform = t;
        UpdateTransforms(child, childTransform);
    }
}