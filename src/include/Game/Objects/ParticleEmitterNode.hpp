#ifndef FE_PARTICLE_EMITTER_NODE_HPP
#define FE_PARTICLE_EMITTER_NODE_HPP

#include "include/Core/Object2D.hpp"
#include "include/Renderer/ParticleSystemNode.hpp"
#include <glm/glm.hpp>
#include <memory>

class ParticleEmitterNode : public Object2D {
public:
	ParticleEmitterNode(const std::unordered_map<std::string, std::any>& data);

	std::string Type() override;
	void Init(std::shared_ptr<Scene> scene) override;
	void Process() override;
	void Burst(int count);

	bool isEmitting;

private:
	std::shared_ptr<ParticleSystemNode> targetSystem;
	float emitTimer;
	float emissionRate;
	float lifeTime;
	glm::vec3 velocityVariation;
	glm::vec3 baseVelocity;
	glm::vec4 startColor;
	std::string targetSystemName;

	void FindSystem(std::shared_ptr<Node> node);
	float RandomFloat(float min, float max);
};

#endif