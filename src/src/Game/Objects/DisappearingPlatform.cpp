#include "include/Game/Objects/DisappearingPlatform.hpp"
#include "include/Globals/Globals.hpp"
#include "include/SceneManager/SceneManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

#include <iostream>

DisappearingPlatform::DisappearingPlatform(const std::unordered_map<std::string, std::any>& data) : Platform(data)
{
    detectOffsetX = fromMap(float, "detectOffsetX", data);
    detectArea = fromMap(float, "detectArea", data);

    this->objectType = ObjectType::Wall;
}

void DisappearingPlatform::Physics(const float& deltaTime) {
    auto& globals = Globals::GetGlobals();
    auto scene = globals.sceneManager->GetActive();
    if (!scene) return;
    auto player = scene->GetPlayer();
    if (!player) return;

    glm::vec3 myPos = GetTransform().GetTranslation();
    glm::vec3 playerPos = player->GetTransform().GetTranslation();

    if (std::abs(playerPos.x - (myPos.x + detectOffsetX)) > detectArea) return;
    if (playerPos.y - myPos.y > 1.5f || playerPos.y - myPos.y < -0.5f) return;

    auto hit = player->Raycast(
        glm::vec2(0.0f, -0.44f),
        glm::vec2(0.0f, -1.0f),
        0.5f,
        static_cast<uint32_t>(ObjectType::Wall)
    );

    if (hit.has_value() && hit->collider->GetOwner().get() == this) {
        std::string id = GetSaveID();
        if (!id.empty() && globals.worldStateManager) {
            globals.worldStateManager->MarkAsDestroyed(globals.activeLevelName, id);
        }
        Globals::GetGlobals().lockPlayerMovement = true;
        Disable();
    }
}