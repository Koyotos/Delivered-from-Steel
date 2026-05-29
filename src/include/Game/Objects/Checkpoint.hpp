#pragma once
#include "include/Core/Object3D.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Game/Objects/Player.hpp"

class Checkpoint : public Object3D {
private:
    std::shared_ptr<Player> player = nullptr;
    std::shared_ptr<Object3D> clothObject = nullptr;
    std::shared_ptr<Light> pointLight = nullptr;

    float activationRadius = 3.0f;
	float colorTransitionProgress = 0.0f;

    bool isActivated = false;
    bool playerInsideAndVisible = false;

    void Activate();

	void MenuToggle();

public:
    Checkpoint(const std::unordered_map<std::string, std::any>& data);
    virtual ~Checkpoint() = default;

    void Init(std::shared_ptr<Scene> scene) override;
    void Physics(const float& deltaTime) override;
    bool Input(InputEvent& event) override;
};
