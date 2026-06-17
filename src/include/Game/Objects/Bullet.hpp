#ifndef FE_BULLET
#define FE_BULLET

#include "include/Game/Objects/Player.hpp"
class Bullet :
    public Object2D
{
private:
    float speed;
	vec2 direction;
    bool isVisible;
	float damage;

	float lifeTime;
	float currentLifeTime = 0.0f;
public:
    Bullet(const std::unordered_map<std::string, std::any>&);

    void Physics(const float& deltaTime) override;
    string Type() override;

    void SetDirection(vec2 newDirection);
    vec2 GetDirection() const;

    void SetVisible(bool visible);
    bool GetVisible() const;

    void OnCollisionEnter(Collider*) override;
    void Attack(std::shared_ptr<Player> player);

	float GetSpeed() const { return speed; }


};

#endif // FE_BULLET
