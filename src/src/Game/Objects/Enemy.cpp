#include "include/Game/Objects/Enemy.hpp"
#include "include/Game/Objects/Player.hpp"

Enemy::Enemy(const unordered_map<string, std::any>& data) : Object2D(data) {
	objectType = ObjectType::Enemy;
}

void Enemy::ChangeState(shared_ptr<Player> player) {
    switch (state) {
    case EnemyState::Patrol: {
        if (seePlayer) {
            state = EnemyState::Chase;
        }
        break;
    }
    case EnemyState::Chase: {
        if (!seePlayer) {
            state = EnemyState::Patrol;
        }
        else {
            float enemyToPlayerDist = glm::length(player->GetTransform().GetTranslation() - transform.GetTranslation());
            if (enemyToPlayerDist < atakDystanse) {
                state = EnemyState::Attack;
            }
        }
        break;
    }
    case EnemyState::Attack: {
		//  to �atwiej bedzie ustawiw� w AttackState(dt);
        break;
    }
    }
}

void Enemy::UpdateState(float dt) {
    switch (state) {
    case EnemyState::Patrol: {
        Patrol(dt);
        break;
    }
    case EnemyState::Chase: {
        Chase(dt);
        break;
    }
    case EnemyState::Attack: {
        AttackState(dt);
        break;
    }
    }
}

void Enemy::OnCollisionStay(shared_ptr<Collider> other) {
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (owner->GetObjectType() == ObjectType::Player) {
		shared_ptr<Player> player = static_pointer_cast<Player>(owner);
		Attack(player);
	}
}

void Enemy::Attack(shared_ptr<Player> player) {
	if (player) {
		player->takeDamage(damage);
	}
}

void Enemy::Init(shared_ptr<Scene> scene) {
	player = scene->GetPlayer();
}

void Enemy::Update(float deltaTime) {
    Patrol(deltaTime);
}

void Enemy::DetectPlayer() {
    if (player) {

        glm::vec3 enemyPos3 = transform.GetTranslation();
        glm::vec3 playerPos3 = player->GetTransform().GetTranslation();

        glm::vec2 enemyPos(enemyPos3);
        glm::vec2 playerPos(playerPos3);

        glm::vec2 dir = playerPos - enemyPos;
        float dist = glm::length(dir);

        if (dist > 0.0001f && dist < 4.0f) {
            dir /= dist;

            auto hit = Raycast(
                glm::vec2(0.0f),
                dir,
                dist,
                ObjectType::Wall
            );

            seePlayer = !hit.has_value();

            //if (seePlayer) {
            //    float speed = 1.0f;

            //    transform.SetTranslation(
            //        transform.GetTranslation() + glm::vec3(dir * speed * deltaTime, 0.0f)
            //    );

            //    SetTransform(transform);
            //}
        }
        else {
			seePlayer = false;
        }
    }
}