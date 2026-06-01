#ifndef FE_ORBITAL_PARTICLE_SYSTEM_HPP
#define FE_ORBITAL_PARTICLE_SYSTEM_HPP

#include "include/Renderer/ParticleSystemNode.hpp"

class OrbitalParticleSystem : public ParticleSystemNode {
public:
	OrbitalParticleSystem(const std::unordered_map<std::string, std::any>& data);

	std::string Type() override;
	void Process() override;
	void UpdateOrbit(float deltaTime, const glm::vec3& playerPos);
	void Reset() { activeParticleCount = 0; points = 0; }
	void UpdatePlayerState(int currentPoints);

	int showThreshold = 15;
	bool unorganizedMode = false;
	float orbitRadius = 1.0f;
	float orbitSpeed = 3.0f;
	float followSpeed = 15.0f;
	float orbitSpreadY = 0.5f;
	glm::vec4 particleColor = glm::vec4(0.2f, 0.8f, 1.0f, 1.0f);

private:
	float elapsedTime = 0.0f;
	glm::vec3 centerPos;
	int points = 0;
};

#endif