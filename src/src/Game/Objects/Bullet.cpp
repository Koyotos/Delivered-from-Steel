#include "include/Game/Objects/Bullet.hpp"


Bullet::Bullet(const std::unordered_map<std::string, std::any>& data)
    : Object2D(data)
{
	objectType = ObjectType::Bullet;
	speed = 3.0f;
	damage = 40.0f;
	direction = vec2(0.0f, 0.0f);
	lifeTime = 2.f;
	SetVisible(false);
}

void Bullet::Physics(const float& deltaTime)
{
	currentLifeTime += deltaTime;
	if (currentLifeTime >= lifeTime) {
		SetVisible(false);
		return;
	}
	if (isVisible) {
		Transform currentTransform = GetTransform();
		currentTransform.SetTranslation(currentTransform.GetTranslation() + vec3(direction * speed * deltaTime, 0.0f));
		SetTransform(currentTransform);
	}
}

string Bullet::Type() {
	return "Bullet";
}

void Bullet::SetDirection(vec2 newDirection)
{
    direction = newDirection;
}

vec2 Bullet::GetDirection() const
{
    return direction;
}

void Bullet::SetVisible(bool visible)
{
    isVisible = visible;
	if (visible) {
		Enable();
		currentLifeTime = 0.0f;
	}
	else {
		Disable();
		SetVelocity(vec2(0.0f));

		Transform t = GetTransform();
		t.SetTranslation(vec3(1000.0f,1000.0f,-5.0f));
		SetTransform(t);

	}
}

bool Bullet::GetVisible() const
{
    return isVisible;
}

void Bullet::OnCollisionEnter(std::shared_ptr<Collider> other)
{
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (owner->GetObjectType() == ObjectType::Player) {
		shared_ptr<Player> player = static_pointer_cast<Player>(owner);
		Attack(player);
	}
	if (owner->GetObjectType() == ObjectType::Wall || 
		owner->GetObjectType() == ObjectType::BreakableWall || 
		owner->GetObjectType() == ObjectType::Trap) {
		SetVisible(false);
	}
}

void Bullet::Attack(std::shared_ptr<Player> player)
{
	player->takeDamage(damage);
	SetVisible(false);
}