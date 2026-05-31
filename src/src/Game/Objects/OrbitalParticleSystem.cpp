#include "include/Game/Objects/OrbitalParticleSystem.hpp"
#include "include/Globals/Globals.hpp"
#include <cmath>

OrbitalParticleSystem::OrbitalParticleSystem(const std::unordered_map<std::string, std::any>& data)
	: ParticleSystemNode(data) {
	if (data.find("showThreshold") != data.end()) {
		showThreshold = static_cast<int>(fromMap(float, "showThreshold", data));
	}
	if (data.find("unorganizedMode") != data.end()) {
		unorganizedMode = fromMap(bool, "unorganizedMode", data);
	}
	if (data.find("orbitRadius") != data.end()) {
		orbitRadius = fromMap(float, "orbitRadius", data);
	}
	if (data.find("orbitSpeed") != data.end()) {
		orbitSpeed = fromMap(float, "orbitSpeed", data);
	}
	if (data.find("followSpeed") != data.end()) {
		followSpeed = fromMap(float, "followSpeed", data);
	}
	if (data.find("orbitSpreadY") != data.end()) {
		orbitSpreadY = fromMap(float, "orbitSpreadY", data);
	}
}

std::string OrbitalParticleSystem::Type() {
	return "OrbitalParticleSystem";
}

void OrbitalParticleSystem::Process() {
	Object2D::Process();
}

void OrbitalParticleSystem::UpdatePlayerState(int currentPoints) {
	points = currentPoints;
	size_t targetCount = (points <= showThreshold) ? points : 0;

	for (size_t i = activeParticleCount; i < targetCount; ++i) {
		colors[i] = particleColor;
		positions[i] = glm::vec4(0.0f, 0.0f, -5.0f, 0.0f);
	}
	activeParticleCount = targetCount;
}

void OrbitalParticleSystem::UpdateOrbit(float deltaTime, const glm::vec3& playerPos) {
	if (activeParticleCount == 0) return;

	elapsedTime += deltaTime;
	float angleStep = 6.28318f / activeParticleCount;
	float lerpFactor = 1.0f - std::exp(-followSpeed * deltaTime);

	for (size_t i = 0; i < activeParticleCount; i++) {
		float targetX, targetY, targetZ;

		if (!unorganizedMode) {
			float angle = elapsedTime * orbitSpeed + (i * angleStep);

			targetX = playerPos.x + cos(angle) * orbitRadius;
			targetY = playerPos.y;
			targetZ = playerPos.z + sin(angle) * orbitRadius;
		}
		else {
			float speedMod = 1.0f + (i % 3) * 0.3f;
			float radiusMod = orbitRadius + sin(elapsedTime * 2.0f + i) * 0.3f;
			float angle = elapsedTime * orbitSpeed * speedMod + (i * 1.37f);

			targetX = playerPos.x + cos(angle) * radiusMod;
			targetY = playerPos.y + sin(elapsedTime * 2.5f + i * 2.0f) * orbitSpreadY;
			targetZ = playerPos.z + sin(angle * 1.2f + i) * radiusMod;
		}

		positions[i].x += (targetX - positions[i].x) * lerpFactor;
		positions[i].y += (targetY - positions[i].y) * lerpFactor;
		positions[i].z += (targetZ - positions[i].z) * lerpFactor;

		positions[i].w += 3.0f * deltaTime;

		colors[i] = particleColor;
	}
}