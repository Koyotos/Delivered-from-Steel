#include "include/Game/Objects/Checkpoint.hpp"
#include "include/Game/Objects/CardManager.hpp"

Checkpoint::Checkpoint(const std::unordered_map<std::string, std::any>& data) : Object3D(data) {

    activationRadius = fromMap(float, "activationRadius", data);

    pointLight = std::make_shared<Light>();
    pointLight->type = LIGHT_POINT;
    pointLight->SetDraw(true);

    pointLight->colorAmbient = glm::vec3(0.0f);
    pointLight->colorSpecular = glm::vec3(0.0f);

	vec3 position = GetTransform().GetTranslation();
	position += vec3(fromMap(float, "lightOffsetX", data), fromMap(float, "lightOffsetY", data), fromMap(float, "lightOffsetZ", data));
    pointLight->data1 = position;

    pointLight->data2 = glm::vec3(1.0f, 0.0f, 1/ ( 4 * activationRadius * activationRadius));

    pointLight->data3 = glm::vec3(0.0f);
    pointLight->data4 = 0.0f;

    pointLight->colorDiffuse = glm::vec3(0.0f, 0.0f, 0.0f);

    AddChild(pointLight);
}
void Checkpoint::Init(std::shared_ptr<Scene> scene) {
    player = scene->GetPlayer();

    for (auto& child : GetChildren()) {
        if (child->Type() == "Object3D") {
            auto castedCloth = std::static_pointer_cast<Object3D>(child);
            clothObject = castedCloth;
            break;
        }
    }
}

std::string Checkpoint::GetSerializeKey() const {
    glm::vec3 pos = GetTransform().GetTranslation();
    return "checkpoint_" + std::to_string((int)pos.x) + "_" + std::to_string((int)pos.y);
}

nlohmann::json Checkpoint::Serialize() const {
    nlohmann::json j;
    j["isActivated"] = isActivated;
    return j;
}

void Checkpoint::Deserialize(const nlohmann::json& data) {
    if (data.contains("isActivated")) {
        isActivated = data["isActivated"];

        if (isActivated) {
            if (clothObject) {
                clothObject->Disable();
            }
        }
    }
}

void Checkpoint::Physics(const float& deltaTime) {
    Object3D::Physics(deltaTime);

    glm::vec2 checkpointPos = GetTransform().GetTranslation();
    glm::vec2 playerPos = player->GetTransform().GetTranslation();

    glm::vec2 dirToPlayer = playerPos - checkpointPos;
    float dist = glm::length(dirToPlayer);

    if (dist <= activationRadius) {
        glm::vec2 normDir = glm::normalize(dirToPlayer);

        glm::vec2 rayDir(normDir.x, normDir.y);

        auto hitWall = Raycast(rayDir, dist, static_cast<uint32_t>(ObjectType::Wall));

        if (!hitWall.has_value() && !playerInsideAndVisible) {
            auto& globals = Globals::GetGlobals();
            if (globals.cardManager) {
                globals.cardManager->ReachCheckpoint();
            }
            if (!isActivated) Activate();
            player->SetRespawnPoint(vec3(checkpointPos,-5));
        }

        playerInsideAndVisible = !hitWall.has_value();
    }
    else {
        playerInsideAndVisible = false;
    }

    if (pointLight && isActivated) {
        if (playerInsideAndVisible) {
            colorTransitionProgress += deltaTime * 2.0f;
            if (colorTransitionProgress > 1.0f) colorTransitionProgress = 1.0f;
        }
        else {
            colorTransitionProgress -= deltaTime * 1.5f;
            if (colorTransitionProgress < 0.0f) colorTransitionProgress = 0.0f;
        }

        glm::vec3 normalColor = glm::vec3(0.0f, 0.6f, 1.5f);

        glm::vec3 activeColor = glm::vec3(0.0f, 0.8f, 2.0f);

        glm::vec3 normalSpecular = glm::vec3(0.0f, 0.02f, 0.1f);
        glm::vec3 activeSpecular = glm::vec3(0.0f, 0.2f, 0.5f);

        pointLight->colorDiffuse = glm::mix(normalColor, activeColor, colorTransitionProgress);
        pointLight->colorSpecular = glm::mix(normalSpecular, activeSpecular, colorTransitionProgress);
    }
}

void Checkpoint::MenuToggle() {
    auto cardManager = Globals::GetGlobals().cardManager;
    if (cardManager) {
		//cardManager->ToggleMenu(); ////////////////////////////// <------------------ odkomentowa�, gdy menu b�dzie gotowe
    }
}

bool Checkpoint::Input(InputEvent& event) {
    if (!event.handled && playerInsideAndVisible) {

        if (event.type == InputType::GAMEPAD_BUTTON && event.action == GLFW_PRESS && event.key == GLFW_GAMEPAD_BUTTON_START) {
            MenuToggle();
            event.handled = true;
            return true;
        }

        if (event.type == InputType::KEYBOARD && event.action == GLFW_PRESS && event.key == GLFW_KEY_E) {
            MenuToggle();
            event.handled = true;
            return true;
        }
    }
    return false;
}

void Checkpoint::Activate() {
    isActivated = true;

    if (clothObject) {
        clothObject->Disable();
    }
}
