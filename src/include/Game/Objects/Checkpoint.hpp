#ifndef FE_CHECKPOINT
#define FE_CHECKPOINT

#include "include/Core/Object3D.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Core/Scene.hpp"
#include "include/AudioManager/AudioSource.hpp"
#include "GLFW/glfw3.h"
#include <memory>

class Checkpoint : public Object3D {
private:
    std::shared_ptr<Player> player = nullptr;
    std::shared_ptr<Object3D> clothObject = nullptr;
    std::shared_ptr<Object2D> infoButtonObject = nullptr;
    std::shared_ptr<Light> pointLight = nullptr;

	unique_ptr<AudioSource> audio = nullptr;

    glm::vec3 buttonNormalScale;

    float activationRadius = 3.0f;
	float colorTransitionProgress = 0.0f;
	float buttonScaleProgress = 0.0f;

    bool isActivated = false;
    bool playerInsideAndVisible = false;

    void Activate();

	void MenuToggle();

public:
    Checkpoint(const std::unordered_map<std::string, std::any>& data);
    virtual ~Checkpoint() = default;
	void Disable() noexcept override;

    void Init(std::shared_ptr<Scene> scene) override;
    void Physics(const float& deltaTime) override;
    bool Input(InputEvent& event) override;
};

#endif