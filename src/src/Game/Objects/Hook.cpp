#include "include/Game/Objects/Hook.hpp"

Hook::Hook(const std::unordered_map<std::string, std::any>& data)
    : Object2D(data),
    speed(0.2f),
    maxDistance(5.0f),
    currentDistance(0.0f),
    direction(vec2(1.0f, 0.0f)),
    isReturning(false),
    hasPlayer(false)
{
}

void Hook::Update(float deltaTime)
{
    Transform transform = GetTransform();
    vec3 position = transform.GetTranslation();

    if (!isReturning)
    {
        vec2 move = direction * speed * deltaTime;
        position += vec3(move, 0.0f);
        currentDistance += length(move);

        if (currentDistance >= maxDistance)
        {
            ReturnToOwner();
        }
    }
    else
    {
        if (auto ownerPtr = owner.lock())
        {
            vec3 ownerPos = ownerPtr->GetTransform().GetTranslation();
            vec3 dir = normalize(ownerPos - position);

            position += dir * speed * deltaTime;

            if (length(ownerPos - position) < 0.2f)
            {
                ReleasePlayer();
            }
        }
    }

    transform.SetTranslation(position);
    SetTransform(transform);

    if (hasPlayer && grabbedPlayer)
    {
        Transform playerTransform = grabbedPlayer->GetTransform();
        playerTransform.SetTranslation(position);
        grabbedPlayer->SetTransform(playerTransform);
    }
}

void Hook::SetDirection(const vec2& dir)
{
    direction = normalize(dir);
}

void Hook::SetSpeed(float newSpeed)
{
    speed = newSpeed;
}

void Hook::SetOwner(std::shared_ptr<Object2D> ownerObj)
{
    owner = ownerObj;
}

void Hook::OnCollisionEnter(std::shared_ptr<Collider> other)
{
    auto ownerNode = other->GetOwner();

    if (ownerNode->GetObjectType() == ObjectType::Player && !hasPlayer)
    {
        auto player = std::static_pointer_cast<Player>(ownerNode);
        GrabPlayer(player);
        ReturnToOwner();
    }
    else
    {
        ReturnToOwner();
    }
}

void Hook::GrabPlayer(std::shared_ptr<Player> player)
{
    grabbedPlayer = player;
    hasPlayer = true;
}

void Hook::ReleasePlayer()
{
    grabbedPlayer.reset();
    hasPlayer = false;
}

void Hook::ReturnToOwner()
{
    isReturning = true;
}