#include "include/Game/Objects/Checkpoint.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/Globals/Globals.hpp"
#include "include/SaveManager/WorldStateManager.hpp"
#include "include/AudioManager/AudioManager.hpp"

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

    pointLight->colorDiffuse = glm::vec3(0.0f);

    pointLight->Disable();

    AddChild(pointLight);
	audio = make_unique<AudioSource>(this);
}

void Checkpoint::Disable() noexcept {
    if (audio) {
        audio->Stop();
	}
    Object3D::Disable();
}

void Checkpoint::Init(std::shared_ptr<Scene> scene) {
    player = scene->GetPlayer();

    for (auto& child : GetChildren()) {
        if (child->Type() == "Object3D") {
            auto castedCloth = std::static_pointer_cast<Object3D>(child);
            clothObject = castedCloth;
        }
        if (child->Type() == "Object2D") {
            auto castedButton = std::static_pointer_cast<Object2D>(child);
            infoButtonObject = castedButton;
			buttonNormalScale = castedButton->GetTransform().GetScale();
        }
    }
}

void Checkpoint::Physics(const float& deltaTime) {
    Object3D::Physics(deltaTime);

    glm::vec2 checkpointPos = GetTransform().GetTranslation();
    glm::vec2 playerPos = player->GetTransform().GetTranslation();

    glm::vec2 dirToPlayer = playerPos - checkpointPos;
    float dist = glm::length(dirToPlayer);

    if (infoButtonObject && clothObject && !clothObject->TestDraw() && !isActivated) {
        Activate();
    }

    if (dist <= activationRadius) {
        glm::vec2 normDir = glm::normalize(dirToPlayer);

        glm::vec2 rayDir(normDir.x, normDir.y);

        auto hitWall = Raycast(rayDir, dist, static_cast<uint32_t>(ObjectType::Wall));

        if (!hitWall.has_value() && !playerInsideAndVisible && player->GetVelocity().y > -7.0f && !player->isDead()) {
            auto& globals = Globals::GetGlobals();
            if (globals.cardManager) {
                globals.cardManager->ReachCheckpoint();
            }
            if (!isActivated) Activate();
			playerInsideAndVisible = true;
            player->SetRespawnPoint(vec3(checkpointPos,-5), globals.activeLevelName);
            Globals::GetGlobals().wantsToSave = true;
            Globals::GetGlobals().Log("Checkpoint activated. Triggering save.");
        }
    }
    else {
        playerInsideAndVisible = false;
    }

    if (pointLight && isActivated && infoButtonObject) {
        if (playerInsideAndVisible) {
            colorTransitionProgress += deltaTime * 2.0f;
            if (colorTransitionProgress > 1.0f) colorTransitionProgress = 1.0f;
			buttonScaleProgress += deltaTime * 7.0f;
            if (buttonScaleProgress > 1.0f) buttonScaleProgress = 1.0f;
        }
        else {
            colorTransitionProgress -= deltaTime * 1.5f;
            if (colorTransitionProgress < 0.0f) colorTransitionProgress = 0.0f;
			buttonScaleProgress -= deltaTime * 7.0f;
			if (buttonScaleProgress < 0.0f) buttonScaleProgress = 0.0f;
        }

        glm::vec3 normalColor = glm::vec3(0.0f, 0.4f, 1.0f);
        glm::vec3 activeColor = glm::vec3(0.0f, 0.8f, 2.0f);

        glm::vec3 normalSpecular = glm::vec3(0.0f, 0.02f, 0.1f);
        glm::vec3 activeSpecular = glm::vec3(0.0f, 0.2f, 0.5f);

        pointLight->colorDiffuse = glm::mix(normalColor, activeColor, colorTransitionProgress);
        pointLight->colorSpecular = glm::mix(normalSpecular, activeSpecular, colorTransitionProgress);

        if (buttonScaleProgress == 0.0f) {
			infoButtonObject->SetDraw(false);
        }
        else {
			infoButtonObject->SetDraw(true);
		    Transform buttonTrans = infoButtonObject->GetTransform();
		    glm::vec3 targetScale = buttonNormalScale * buttonScaleProgress;
			buttonTrans.SetScale(targetScale);
			infoButtonObject->SetTransform(buttonTrans);
        }

    }
    if (audio) {
		audio->Update();
    }
}

void Checkpoint::MenuToggle() {
    auto cardManager = Globals::GetGlobals().cardManager;
    if (cardManager) {
		cardManager->ToggleMenu();
    }
}

bool Checkpoint::Input(InputEvent& event) {
    if (!event.handled && playerInsideAndVisible) {

        if (event.type == InputType::GAMEPAD_BUTTON && event.action == GLFW_PRESS && (event.key == GLFW_GAMEPAD_BUTTON_START || event.key == GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER)) {
            MenuToggle();
            event.handled = true;
            return true;
        }

        if (event.type == InputType::KEYBOARD && event.action == GLFW_PRESS && (event.key == GLFW_KEY_E || event.key == GLFW_KEY_TAB)) {
            MenuToggle();
            event.handled = true;
            return true;
        }
    }
    return false;
}

void Checkpoint::Activate() {
    isActivated = true;

    audio->PlayLooping("cool_ass_dzwiek", 0.1f, 1.0f, 7.5f, 0.8f);
    pointLight->Enable();
    if (clothObject && clothObject->TestDraw()) {
        std::string id = clothObject->GetSaveID();
        if (!id.empty()) {
            auto& globals = Globals::GetGlobals();
            if (globals.worldStateManager) {
                std::string currentLevel = globals.activeLevelName;
                if (id.find("base_") == 0) {
                    currentLevel = "base";
                }
                globals.worldStateManager->MarkAsDestroyed(currentLevel, id);
            }
        }
        if (auto aum = Globals::GetGlobals().audioManager) {
            aum->PlaySound3D("cloth_pulled", GetTransform().GetTranslation(), 3.0f, 1.0f);
        }
        clothObject->Disable();
    }
}
