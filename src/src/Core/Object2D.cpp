#include "include/Core/Object2D.hpp"

void Object2D::Draw(shared_ptr<Shader> sh) {
    if(sh == nullptr) {
        sh = shader;
    }
    sh->SetMat4("M", transform.GetGlobal());
    if(reqPerspecive) {
        sh->SetVec2("spriteSize", sprite->GetSize());
    }
    sprite->Draw(*sh, currentTextureID);
}

void Object2D::Process() {
    PhysicsNode::Process();

    if (animPlaying && sprite) {
        animTimer += Globals::GetGlobals().GetDeltaTime();
        if (animTimer >= animSpeed) {
            animTimer -= animSpeed;

            const AnimationData* anim = sprite->GetAnimation(currentAnimation);
            if (anim && !anim->frames.empty()) {
                currentFrameIndex++;
                if (currentFrameIndex >= anim->frames.size()) {
                    if (animLoop) {
                        currentFrameIndex = 0;
                    }
                    else {
                        currentFrameIndex = anim->frames.size() - 1;
                        animPlaying = false;
                    }
                }
                currentTextureID = anim->frames[currentFrameIndex];
            }
        }
    }
}

void Object2D::Play(const string& animName, float frameDuration, bool loop) {
    if (!sprite || !sprite->HasAnimation(animName)) return;
    if (currentAnimation == animName && animPlaying) return;

    currentAnimation = animName;
    animSpeed = frameDuration;
    animLoop = loop;
    animPlaying = true;
    animTimer = 0.0f;
    currentFrameIndex = 0;

    const AnimationData* anim = sprite->GetAnimation(currentAnimation);
    if (anim && !anim->frames.empty()) {
        currentTextureID = anim->frames[currentFrameIndex];
    }
}

void Object2D::Stop() {
    animPlaying = false;
}

bool Object2D::IsPlaying() const {
    return animPlaying;
}

string Object2D::GetCurrentAnimation() const {
    return currentAnimation;
}

string Object2D::Type() {
    return "Object2D";
}

uint8_t Object2D::RenderType() {
    return 3;
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
    animPlaying = false;
    currentFrameIndex = 0;
    animTimer = 0.0f;
    animSpeed = 0.1f;
    animLoop = false;
}

Object2D::Object2D(const unordered_map<string, std::any>& data) : PhysicsNode(data) {
    reqPerspecive = fromMap(bool, "reqPerspective", data);
    animPlaying = false;
    currentFrameIndex = 0;
	currentTextureID = 0;
    animTimer = 0.0f;
    animSpeed = 0.1f;
    animLoop = false;
}