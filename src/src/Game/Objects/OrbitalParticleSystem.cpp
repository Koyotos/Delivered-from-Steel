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
	size_t targetOrbitCount = (currentPoints <= showThreshold) ? currentPoints : 0;

	if (targetOrbitCount < points) {
		for (size_t i = targetOrbitCount; i < points; ++i) {
			float vx = ((rand() % 100) / 50.0f) - 1.0f;
			float vy = ((rand() % 100) / 50.0f) + 1.0f;
			float vz = ((rand() % 100) / 50.0f) - 1.0f;

			velocities[i] = glm::vec3(vx, vy, vz) * 2.0f;
			lives[i] = 1.0f;
		}
	}

	else if (targetOrbitCount > points) {
		for (size_t i = points; i < targetOrbitCount; ++i) {
			if (activeParticleCount < maxParticles) {
				positions[activeParticleCount] = positions[i];
				velocities[activeParticleCount] = velocities[i];
				lives[activeParticleCount] = lives[i];
				colors[activeParticleCount] = colors[i];
				activeParticleCount++;
			}
			positions[i] = glm::vec4(0.0f, 0.0f, -5.0f, 0.0f);
		}
	}

	points = targetOrbitCount;
}

void OrbitalParticleSystem::UpdateOrbit(float deltaTime, const glm::vec3& playerPos) {
	if (activeParticleCount == 0) return;

	elapsedTime += deltaTime;
	float angleStep = 6.28318f / (points > 0 ? points : 1);
	float lerpFactor = 1.0f - std::exp(-followSpeed * deltaTime);

	for (size_t i = 0; i < points; i++) {
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

	for (size_t i = points; i < activeParticleCount; ) {
		lives[i] -= deltaTime;

		if (lives[i] <= 0.0f) {
			size_t lastIdx = activeParticleCount - 1;
			positions[i] = positions[lastIdx];
			velocities[i] = velocities[lastIdx];
			lives[i] = lives[lastIdx];
			colors[i] = colors[lastIdx];
			activeParticleCount--;
		}
		else {
			velocities[i].y -= gravity * deltaTime;
			velocities[i] -= velocities[i] * damping * deltaTime;

			positions[i].x += velocities[i].x * deltaTime;
			positions[i].y += velocities[i].y * deltaTime;
			positions[i].z += velocities[i].z * deltaTime;

			positions[i].w += 15.0f * deltaTime;
			i++;
		}
	}
}