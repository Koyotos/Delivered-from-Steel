#include "include/Core/Object2D.hpp"

void Object2D::Draw() {
    shader->SetMat4("M", transform.GetGlobal());
    if(reqPerspecive) {
        shader->SetVec2("spriteSize", sprite->GetSize());
    }
    sprite->Draw(*shader);
}

shared_ptr<Sprite> Object2D::GetSprite() {
    return sprite;
}

void Object2D::SetSprite(shared_ptr<Sprite> sprite) {
    this->sprite = sprite;
}

const bool& Object2D::GetReqPerspective() {
    return reqPerspecive;
}

void Object2D::SetReqPerspective(const bool& state) {
    reqPerspecive = state;
}

Object2D::Object2D() {
    reqPerspecive = false;
}

Object2D::Object2D(const unordered_map<string, std::any>& data) : VisualNode(data) {
    reqPerspecive = fromMap(bool,"reqPerspective",data);
}