#include "include/Core/Object3D.hpp"

void Object3D::Draw() {
    shader->SetMat4("M", transform.GetGlobal());
    model->Draw(*shader);
}

shared_ptr<Model> Object3D::GetModel() {
    return model;
}

void Object3D::SetModel(shared_ptr<Model> model) {
    this->model = model;
}

Object3D::Object3D() {

}

Object3D::Object3D(const unordered_map<string, std::any>& data) : PhysicsNode(data) {
    
}