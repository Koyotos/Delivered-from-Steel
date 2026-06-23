#include "include/Game/Objects/PickUpAbstract.hpp"

#include "include/Globals/Globals.hpp"
#include "include/Game/Objects/CardManager.hpp"
#include "include/SaveManager/WorldStateManager.hpp"

PickUpAbstract::PickUpAbstract() : Object2D(), isCollected(false) {}

PickUpAbstract::PickUpAbstract(const unordered_map<string, std::any>& data) : Object2D(data), isCollected(false) {
	float lightRadius = 0.3f;

    pointLight = std::make_shared<Light>();
    pointLight->type = LIGHT_POINT;
    pointLight->SetDraw(true);

    pointLight->colorAmbient = glm::vec3(0.0f);
    pointLight->colorSpecular = specular1;

    pointLight->data1 = GetTransform().GetTranslation();

    pointLight->data2 = glm::vec3(1.0f, 0.0f, 1 / (4 * lightRadius * lightRadius));

    pointLight->data3 = glm::vec3(0.0f);
    pointLight->data4 = 0.0f;

    pointLight->colorDiffuse = color1;

    AddChild(pointLight);
    audio = make_unique<AudioSource>(this);
}

PickUpAbstract::~PickUpAbstract() {}

void PickUpAbstract::Disable() noexcept {
    if (audio) {
        audio->Stop();
	}
    Object2D::Disable();
}

void PickUpAbstract::Physics(const float& deltaTime) {
    if (isCollected) {
		pointLight->Disable();
        if (audio) {
            audio->Stop();
		}
        return;
    }
    if (pointLight) {
        float tempColorTransitionProgress = 0.0f;

        colorTransitionProgress += deltaTime * 2.0f;
        if (colorTransitionProgress > 1.0f) {
            if (colorTransitionProgress > 2.0f) {
                colorTransitionProgress -= 2.0f;
                tempColorTransitionProgress = colorTransitionProgress;
            }
            else
                tempColorTransitionProgress = 2.0f - colorTransitionProgress;
        }
        else {
            tempColorTransitionProgress = colorTransitionProgress;
        }


        pointLight->colorDiffuse = glm::mix(color1, color2, tempColorTransitionProgress);
        pointLight->colorSpecular = glm::mix(specular1, specular2, tempColorTransitionProgress);
    }
    if (audio) {
        audio->PlayLooping(loopSoundName, 0.55f, 1.0f, 6.5f, 1.0f);
		audio->Update();
    }
}

void PickUpAbstract::OnCollisionEnter(Collider* other) {
    if (isCollected) return;

    std::shared_ptr<PhysicsNode> owner = other->GetOwner();
    if (!owner) return;
    if (owner && owner->GetObjectType() == ObjectType::Player) {
        isCollected = true;
        pointLight->Disable();

		OnPickUp();

        std::string id = this->GetSaveID();
        if (!id.empty()) {
            auto& globals = Globals::GetGlobals();
            if (globals.worldStateManager) {
                std::string currentLevel = globals.activeLevelName;
                globals.worldStateManager->MarkAsDestroyed(currentLevel, id);
            }
        }

        this->Disable();
    }
}


void PickUpAbstract::Init(shared_ptr<Scene> scene) {
    player = scene->GetPlayer();
}

void PickUpAbstract::OnPickUp()
{
}