#ifndef FE_HOOK
#define FE_HOOK

#include "include/Game/Objects/Player.hpp"

class Hook : public Object2D
{
private:
    float speed;
    float maxDistance;
    float currentDistance;

    vec2 direction;

    bool isReturning;
    bool hasPlayer;

    std::shared_ptr<Player> grabbedPlayer;
    std::weak_ptr<Object2D> owner;

public:
    Hook(const std::unordered_map<std::string, std::any>&);

    void Update(float deltaTime) override;

    void SetDirection(const vec2& dir);
    void SetSpeed(float newSpeed);

    void SetOwner(std::shared_ptr<Object2D> ownerObj);

    void OnCollisionEnter(std::shared_ptr<Collider> other) override;

    void GrabPlayer(std::shared_ptr<Player> player);
    void ReleasePlayer();

    void ReturnToOwner();
};

#endif // FE_HOOK