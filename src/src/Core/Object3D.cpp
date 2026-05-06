#include "include/Core/Object3D.hpp"

void Object3D::Draw(shared_ptr<Shader> sh) {
    if(sh == nullptr) {
        sh = shader;
    }
    sh->SetMat4("M", transform.GetGlobal());
    model->Draw(*sh);
}

string Object3D::Type() {
    return "Object3D";
}

uint8_t Object3D::RenderType() {
    return 4;
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