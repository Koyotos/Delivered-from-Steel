#include "include/Game/Objects/ParticleEmitterNode.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Core/Scene.hpp"
#include <random>

ParticleEmitterNode::ParticleEmitterNode(const std::unordered_map<std::string, std::any>& data)
	: Object2D(data), emitTimer(0.0f) {

	isEmitting = fromMap(bool, "isEmitting", data);
	emissionRate = fromMap(float, "emissionRate", data);
	lifeTime = fromMap(float, "lifeTime", data);

	float baseVx = fromMap(float, "baseVelocityX", data);
	float baseVy = fromMap(float, "baseVelocityY", data);
	if (data.find("baseVelocityY") == data.end()) baseVy = 1.0f;
	baseVelocity = glm::vec3(baseVx, baseVy, 0.0f);

	float varX = fromMap(float, "variationX", data);
	float varY = fromMap(float, "variationY", data);
	if (data.find("variationX") == data.end()) varX = 0.5f;
	if (data.find("variationY") == data.end()) varY = 0.5f;
	velocityVariation = glm::vec3(varX, varY, 0.0f);
	startColor = glm::vec4(1.0f);
}

std::string ParticleEmitterNode::Type() {
	return "ParticleEmitterNode";
}

void ParticleEmitterNode::FindSystem(std::shared_ptr<Node> node) {
	if (!targetSystem) {
		if (auto sys = std::dynamic_pointer_cast<ParticleSystemNode>(node)) {
			targetSystem = sys;
			return;
		}
		for (auto& child : node->GetChildren()) {
			FindSystem(child);
		}
	}
}

void ParticleEmitterNode::Init(std::shared_ptr<Scene> scene) {
	if (scene && scene->GetRoot()) {
		FindSystem(scene->GetRoot());
	}
}

float ParticleEmitterNode::RandomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
}

void ParticleEmitterNode::Process() {
	Object2D::Process();
	SetTransform(GetTransform());
	if (!isEmitting || !targetSystem) return;

	float dt = Globals::GetGlobals().GetDeltaTime();
	emitTimer += dt;
	float interval = 1.0f / emissionRate;

	while (emitTimer >= interval) {
		emitTimer -= interval;
		Burst(1);
	}
}

void ParticleEmitterNode::Burst(int count) {
	if (!targetSystem) return;
	glm::mat4 globalMat = GetTransform().GetGlobal();
	glm::vec3 spawnPos = glm::vec3(globalMat[3][0], globalMat[3][1], globalMat[3][2]);

	for (int i = 0; i < count; i++) {
		glm::vec3 randomVelocity = baseVelocity + glm::vec3(
			RandomFloat(-velocityVariation.x, velocityVariation.x),
			RandomFloat(-velocityVariation.y, velocityVariation.y),
			0.0f
		);
		float randomRot = RandomFloat(0.0f, 6.28f);
		targetSystem->Emit(spawnPos, randomVelocity, startColor, lifeTime, randomRot);
	}
}