#include "include/Core/Transform.hpp"

void Transform::UpdateLocal() {
    mat4 T = glm::translate(mat4(1.0f), translation);
    mat4 R = glm::toMat4(rotation);
    mat4 S = glm::scale(mat4(1.0f), scale);

    local = T * R * S;
    global = local;
}

const mat4& Transform::GetLocal() const {
    return local;
}

const mat4& Transform::GetGlobal() const {
    return global;
}

void Transform::SetGlobal(mat4 g) {
    global = g;
}

void Transform::Combine(const Transform& parent) {
    global = parent.global*local;
}

const vec3& Transform::GetTranslation() const {
    return translation;
}

const vec3 Transform::GetRotation() const {
    return eulerAngles(rotation);
}

const vec3& Transform::GetScale() const {
    return scale;
}

void Transform::SetTranslation(const vec3& translation) {
    this->translation = translation;
    UpdateLocal();
}

void Transform::SetRotation(const vec3& rotation) {
    this->rotation = quat(rotation);
    UpdateLocal();
}

void Transform::SetScale(const vec3& scale) {
    this->scale = scale;
    UpdateLocal();
}

void Transform::ResetGlobal() {
    global = local;
}

Transform::Transform() {
    translation = vec3(0,0,0);
    rotation = quat(vec3(0,0,0));
    scale = vec3(1,1,1);
    UpdateLocal();
}

Transform::Transform(const vec3& translation, const vec3& rotation, const vec3& scale) {
    this->translation = translation;
    this->rotation = quat(rotation);
    this->scale = scale;
    UpdateLocal();
}

Transform::Transform(const vector<std::any>& data) {
    translation.x = any_cast<float>(data[0]);
    translation.y = any_cast<float>(data[1]);
    translation.z = any_cast<float>(data[2]);

    vec3 rot(any_cast<float>(data[3]), any_cast<float>(data[4]), any_cast<float>(data[5]));
    rotation = quat(rot);

    scale.x = any_cast<float>(data[6]);
    scale.y = any_cast<float>(data[7]);
    scale.z = any_cast<float>(data[8]);
    UpdateLocal();
}

Transform::~Transform() {

}

