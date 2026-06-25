#ifndef FE_PICK_UP_ABSTRACT
#define FE_PICK_UP_ABSTRACT

#include "include/Game/Objects/Player.hpp"
#include "include/Renderer/Light.hpp"
#include "include/AudioManager/AudioSource.hpp"
#include <memory>

class PickUpAbstract : public Object2D {
protected:
    shared_ptr<Player> player;
    bool isCollected = false;
    float colorTransitionProgress = 0.0f;
    std::shared_ptr<Light> pointLight = nullptr;

    glm::vec3 color1 = glm::vec3(0.0f, 0.4f, 1.0f);
    glm::vec3 color2 = glm::vec3(0.0f, 0.8f, 2.0f);

    glm::vec3 specular1 = glm::vec3(0.0f, 0.02f, 0.1f);
    glm::vec3 specular2 = glm::vec3(0.0f, 0.2f, 0.5f);
	string loopSoundName = "";
	unique_ptr<AudioSource> audio = nullptr;
public:
    PickUpAbstract();
    PickUpAbstract(const unordered_map<string, std::any>& data);
    virtual ~PickUpAbstract();
	void Disable() noexcept override;

    void Init(shared_ptr<Scene> scene) override;

    void OnCollisionEnter(Collider* other) override;

    virtual void OnPickUp();

    void Physics(const float& deltaTime) override;
};
#endif // FE_PICK_UP_ABSTRACT