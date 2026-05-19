#include "include/Game/Objects/Drone.hpp"
#include "include/Globals/Globals.hpp"
#include "include/AudioManager/AudioManager.hpp"
#include "include/Core/Scene.hpp"
#include "include/SaveManager/WorldStateManager.hpp"
#include "include/SceneManager/SceneManager.hpp"
#include <glm/geometric.hpp>

Drone::Drone(const unordered_map<string, std::any>& data) : Enemy(data) {
	direction = static_cast<int>(fromMap(int64_t, "direction", data));
	patrolSpeed = fromMap(float, "patrolSpeed", data);
	patrolDistance = fromMap(float, "patrolDistance", data);
	diveSpeed = fromMap(float, "diveSpeed", data);
	visionRange = fromMap(float, "visionRange", data);
	visionAngle = fromMap(float, "visionAngle", data);
	explosionRadius = fromMap(float, "explosionRadius", data);
	explosionDamage = fromMap(float, "explosionDamage", data);
	turnSpeed = fromMap(float, "turnSpeed", data);

	spotLight = make_shared<Light>();
	spotLight->type = LIGHT_SPOT;
	spotLight->colorAmbient = vec3(0.0f, 0.0f, 0.0f);
	spotLight->colorSpecular = vec3(0.0f, 0.0f, 0.0f);
	spotLight->data1 = GetTransform().GetTranslation();
	//spotLight->data4 = glm::radians(visionAngle);
	spotLight->data4 = glm::cos(glm::radians(visionAngle) / 2.0f); 
	spotLight->data2 = vec3(0.001f, -1.0f, 0.0f);
	spotLight->colorDiffuse = vec3(1.0f, 0.6f, 0.8f);
	spotLight->data3 = vec3(1.0f, 0.09f, 0.032f);

	AddChild(spotLight);
}

void Drone::Init(shared_ptr<Scene> scene) {
	Enemy::Init(scene);
	startPos = GetTransform().GetTranslation();
	targetPos = startPos + vec3(direction * patrolDistance, 0.0f, 0.0f);
	endPos = targetPos;
}

void Drone::Physics(const float& deltaTime) {
	if (spotLight) {
		spotLight->data1 = GetTransform().GetTranslation();
	}
	vec2 currentVel = GetVelocity();
	SetVelocity(vec2(currentVel.x, 10.0f * deltaTime));
	Enemy::Physics(deltaTime);
}

void Drone::DetectPlayer() {
	if (!player) return;

	if (state == EnemyState::Chase) return;

	vec2 dronePos = GetTransform().GetTranslation();
	vec2 playerPos = player->GetTransform().GetTranslation();
	vec2 dirToPlayer = playerPos - dronePos;
	float dist = glm::length(dirToPlayer);

	if (dist > visionRange) {
		seePlayer = false;
		return;
	}

	dirToPlayer /= dist;
	vec2 lookDir(0.0f, -1.0f);
	float angleToPlayer = glm::degrees(glm::acos(glm::dot(lookDir, dirToPlayer)));

	if (angleToPlayer <= visionAngle / 2.0f) {
		auto hitWall = Raycast(dirToPlayer, dist, obstacleMask);
		if (!hitWall.has_value()) {
			seePlayer = true;
			diveTarget = vec3(playerPos.x, playerPos.y, GetTransform().GetTranslation().z);
		}
		else {
			seePlayer = false;
		}
	}
	else {
		seePlayer = false;
	}
}

void Drone::ChangeState(shared_ptr<Player> p) {
	if (state == EnemyState::Patrol && seePlayer) {
		state = EnemyState::Chase;
		spotLight->colorDiffuse = vec3(1.0f, 0.0f, 0.0f);
		vec3 currentPos = GetTransform().GetTranslation();
		vec2 dir = vec2(diveTarget.x, diveTarget.y) - vec2(currentPos.x, currentPos.y);
		currentDiveVelocity = glm::normalize(dir) * diveSpeed;
	}
}

void Drone::Chase(float dt) {
	SetVelocity(vec2(0.0f, 0.0f));

	if (!player) return;

	vec3 currentPos = GetTransform().GetTranslation();
	vec3 playerPos = player->GetTransform().GetTranslation();
	vec2 desiredDir = vec2(playerPos.x, playerPos.y) - vec2(currentPos.x, currentPos.y);
	float distToPlayer = glm::length(desiredDir);

	if (distToPlayer > 0.001f) {
		desiredDir /= distToPlayer;
	}
	vec2 desiredVelocity = desiredDir * diveSpeed;

	currentDiveVelocity = glm::mix(currentDiveVelocity, desiredVelocity, turnSpeed * dt);
	vec2 diveDir = glm::normalize(currentDiveVelocity);
	currentDiveVelocity = diveDir * diveSpeed;

	if (distToPlayer < explosionRadius * 0.4f) {
		Explode();
		return;
	}
	vec3 moveStep = vec3(currentDiveVelocity.x, currentDiveVelocity.y, 0.0f) * dt;

	Transform t = GetTransform();
	t.SetTranslation(currentPos + moveStep);
	direction = (currentDiveVelocity.x > 0) ? 1 : -1;
	glm::vec3 scale = t.GetScale();
	scale.x = std::abs(scale.x) * direction;
	t.SetScale(scale);
	SetTransform(t);
}

void Drone::Patrol(float dt) {
	SetVelocity(vec2(0.0f, 0.0f));

	vec3 currentPos = GetTransform().GetTranslation();
	vec3 dir = targetPos - currentPos;
	dir.y = 0.0f;
	float dist = glm::length(dir);

	if (dist < 0.1f) {
		ReversePatrol();
		return;
	}

	dir /= dist;
	auto hitWall = Raycast(vec2(dir.x, 0.0f), 0.2f, obstacleMask);
	if (hitWall.has_value()) {
		ReversePatrol();
		return;
	}

	constexpr float slowdownRadius = 1.0f;
	constexpr float minSpeed = 0.5f;
	float currentSpeed = patrolSpeed;
	if (dist < slowdownRadius) {
		float t = dist / slowdownRadius;
		currentSpeed = minSpeed + (patrolSpeed - minSpeed) * t;
	}

	vec3 moveStep = dir * currentSpeed * dt;
	Transform t = GetTransform();
	t.SetTranslation(currentPos + moveStep);

	direction = (dir.x > 0) ? 1 : -1;
	glm::vec3 scale = t.GetScale();
	scale.x = std::abs(scale.x) * direction;
	t.SetScale(scale);
	SetTransform(t);
}

void Drone::ReversePatrol() {
	if (glm::distance(targetPos, startPos) < 0.1f) {
		targetPos = endPos;
	}
	else {
		targetPos = startPos;
	}
}

void Drone::OnCollisionEnter(shared_ptr<Collider> other) {
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (owner->GetObjectType() == ObjectType::Wall) {
		Explode();
	}
	if (owner->GetObjectType() == ObjectType::BreakableWall) {
		Explode();
	}
	if (owner->GetObjectType() == ObjectType::Enemy) {
		Explode();
	}
	if (owner->GetObjectType() == ObjectType::Trap) {
		Explode();
	}
}

void Drone::Explode() {
	if (player) {
		float distToPlayer = glm::length(player->GetTransform().GetTranslation() - GetTransform().GetTranslation());
		if (distToPlayer <= explosionRadius) {
			player->takeDamage(explosionDamage);
		}
	}

	//if (Globals::GetGlobals().audioManager) {
	//	Globals::GetGlobals().audioManager->PlaySound3D("drone_explode", GetTransform().GetTranslation());
	//}

	if (spotLight) spotLight->Disable();
	Transform t = GetTransform();
	vec3 outOfBoundsPos(-9999.0f, -9999.0f, -9999.0f);
	t.SetTranslation(outOfBoundsPos);
	SetTransform(t);
	std::string id = this->GetSaveID();
	if (!id.empty()) {
		auto& globals = Globals::GetGlobals();
		if (globals.sceneManager && globals.worldStateManager) {
			std::string currentSceneName = globals.sceneManager->GetActive()->GetName();
			globals.worldStateManager->MarkAsDestroyed(currentSceneName, id);
		}
	}
	Disable();
}