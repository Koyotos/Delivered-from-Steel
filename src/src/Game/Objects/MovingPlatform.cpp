#include "include/Game/Objects/MovingPlatform.hpp"
#include "include/PhysicsManager/CapsuleCollider.hpp"
#include "include/PhysicsManager/BoxCollider.hpp"
#include "include/Core/Scene.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Game/Objects/Enemy.hpp"
#include "include/AudioManager/AudioManager.hpp"

MovingPlatform::MovingPlatform(const unordered_map<string, std::any>& data) : Platform(data) {
	state = MovingPlatformState::StopStart;
	MovingDuration = fromMap(float, "movingDuration", data);
	StopDuration = fromMap(float, "stopDuration", data);
	startPosition = GetTransform().GetTranslation();
	endPosition = startPosition + vec3( fromMap(float, "XendPosition", data), fromMap(float, "YendPosition",data), 0.0f) ;
	timer = fromMap(float, "movingOffset", data);
	audio = make_unique<AudioSource>(this);
}

void MovingPlatform::Disable() noexcept {
    if (audio) {
        audio->Stop();
	}
    Platform::Disable();
}

vec3 Lerp(const vec3& a, const vec3& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return a + (b - a) * t;
}   

void MovingPlatform::Init(shared_ptr<Scene> scene) {
    player = scene->GetPlayer();
}

void MovingPlatform::Physics(const float& deltaTime) {
    timer += deltaTime;

    vec3 from = (state == MovingPlatformState::MovingToEnd) ? startPosition : endPosition;
    vec3 to = (state == MovingPlatformState::MovingToEnd) ? endPosition : startPosition;

    float moveDuration = (state == MovingPlatformState::MovingToEnd) ? MovingDuration : MovingDuration;
    float pauseDuration = (state == MovingPlatformState::StopEnd) ? StopDuration : StopDuration;

    switch (state)
    {
    case MovingPlatformState::StopStart:
    case MovingPlatformState::StopEnd:
        if (timer >= pauseDuration) {
            state = (state == MovingPlatformState::StopEnd)
                ? MovingPlatformState::MovingToStart
                : MovingPlatformState::MovingToEnd;
            timer -= pauseDuration;
        }
        if (audio) {
            audio->Stop();
        }
        break;

    case MovingPlatformState::MovingToEnd:
    case MovingPlatformState::MovingToStart:
    {
        float t = timer / moveDuration;

        Transform trans = GetTransform();

        trans.SetTranslation(Lerp(from, to, t));

        SetTransform(trans);
        ResetGlobal();

        if (timer >= moveDuration) {
            state = (state == MovingPlatformState::MovingToStart)
                ? MovingPlatformState::StopStart
                : MovingPlatformState::StopEnd;
            timer -= moveDuration;
        }
        if (audio) {
            audio->PlayLooping("moving_platform", 0.08f, 0.5f, 7.5f, 2.0f);
        }
        break;
    }
    }

    vec3 newPosition = GetTransform().GetTranslation();
    velocityDelta = newPosition - lastPosition;
    velocity = velocityDelta / deltaTime;

    SetVelocity(vec2(velocity.x, 0));

    lastPosition = newPosition;

    if (audio) {
		audio->Update();
    }
}


void MovingPlatform::OnCollisionStay(Collider* other) {
    shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (!owner) return;
    if (owner->GetObjectType() == ObjectType::Player) {
        std::shared_ptr<Player> player = std::static_pointer_cast<Player>(owner);
        if (player) {
            CapsuleCollider* capsule = static_cast<CapsuleCollider*>(other);
            BoxCollider* box = static_cast<BoxCollider*>(GetCollider());

            float playerBottom = player->GetTransform().GetTranslation().y - (capsule->radius + capsule->height/2);
            float platformTop = box->GetMax().y;

            if (playerBottom >= platformTop || player->IsWallSliding()) {
                Transform trans = player->GetTransform();

                vec3 pos = trans.GetTranslation();
                pos += velocityDelta;

                trans.SetTranslation(pos);
                player->SetTransform(trans);
            }
        }
    }

    if (owner->GetObjectType() == ObjectType::Enemy) {
        std::shared_ptr<Enemy> enemyNode = std::static_pointer_cast<Enemy>(owner);
        if (enemyNode) {
            Transform trans = enemyNode->GetTransform();

            vec3 pos = trans.GetTranslation();
            pos += velocityDelta;

            trans.SetTranslation(pos);
            enemyNode->SetTransform(trans);
        }
    }
}

void MovingPlatform::OnCollisionExit(Collider* other) {
    shared_ptr<PhysicsNode> owner = other->GetOwner();
    if (!owner) return;
    if (owner->GetObjectType() == ObjectType::Player) {
        std::shared_ptr<Player> player = std::static_pointer_cast<Player>(owner);
        if (player) {

            CapsuleCollider* capsule = static_cast<CapsuleCollider*>(other);

            float playerBottom = player->GetTransform().GetTranslation().y - (capsule->radius * 2 + capsule->height);
			float platformTop = static_cast<BoxCollider*>(GetCollider())->GetMax().y;

            if (playerBottom >= platformTop) {
                Transform trans = player->GetTransform();

                vec3 pos = trans.GetTranslation();

                pos += vec3(0.0f, velocityDelta.y * 1.0f,0.0f);

                trans.SetTranslation(pos);
                player->SetTransform(trans);
            }

            if (playerBottom + 0.05f >= platformTop || player->IsWallSliding()) {
                Transform trans = player->GetTransform();

                vec3 pos = trans.GetTranslation();

                pos += vec3(velocityDelta.x * 1.0f, velocityDelta.y * 1.0f, 0.0f);

                trans.SetTranslation(pos);
                player->SetTransform(trans);
                player->addPlatformVelocity(velocity * 1.0f);
            }
        }
    }
}