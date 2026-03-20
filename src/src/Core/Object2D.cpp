#include "include/Core/Object2D.hpp"

void Object2D::Draw() {
    shader->SetMat4("M", transform.GetGlobal());
    sprite->Draw(*shader);
}

shared_ptr<Sprite> Object2D::GetSprite() {
    return sprite;
}

void Object2D::SetSprite(shared_ptr<Sprite> sprite) {
    this->sprite = sprite;
}