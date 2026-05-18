#ifndef FE_PARTICLESYSTEM_NODE_HPP
#define FE_PARTICLESYSTEM_NODE_HPP

#include "include/Core/Object2D.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class ParticleSystemNode : public Object2D {
public:
	ParticleSystemNode(const std::unordered_map<std::string, std::any>& data);
	~ParticleSystemNode();

	std::string Type() override;

	void Process() override;
	void Draw(std::shared_ptr<Shader> sh = nullptr) override;
	void Emit(const glm::vec3& position, const glm::vec3& velocity, const glm::vec4& color, float lifeTime, float rotation = 0.0f);

private:
	size_t maxParticles;
	size_t activeParticleCount;

	float gravity;
	float damping;

	std::vector<glm::vec4> positions;
	std::vector<glm::vec3> velocities;
	std::vector<glm::vec4> colors;
	std::vector<float> lives;

	GLuint VAO;
	GLuint quadVBO;
	GLuint instanceVBO_Pos;
	GLuint instanceVBO_Col;

	void SetupGL();
};

#endif