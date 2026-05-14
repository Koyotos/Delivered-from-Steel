#include "include/Renderer/ParticleSystemNode.hpp"
#include "include/Globals/Globals.hpp"

ParticleSystemNode::ParticleSystemNode(const std::unordered_map<std::string, std::any>& data)
	: Object2D(data), activeParticleCount(0) {

	maxParticles = fromMap(float, "maxParticles", data);
	if (maxParticles == 0) maxParticles = 10000;
	gravity = fromMap(float, "gravity", data);
	damping = fromMap(float, "damping", data);

	positions.resize(maxParticles);
	velocities.resize(maxParticles);
	colors.resize(maxParticles);
	lives.resize(maxParticles);

	SetReqPerspective(true);
	SetupGL();
}

ParticleSystemNode::~ParticleSystemNode() {
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &instanceVBO_Pos);
	glDeleteBuffers(1, &instanceVBO_Col);
	glDeleteVertexArrays(1, &VAO);
}

std::string ParticleSystemNode::Type() {
	return "ParticleSystemNode";
}

void ParticleSystemNode::SetupGL() {
	float quadVertices[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glGenBuffers(1, &instanceVBO_Pos);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Pos);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glVertexAttribDivisor(1, 1);

	glGenBuffers(1, &instanceVBO_Col);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Col);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glVertexAttribDivisor(2, 1);

	glBindVertexArray(0);
}

void ParticleSystemNode::Emit(const glm::vec3& position, const glm::vec3& velocity, const glm::vec4& color, float lifeTime, float rotation) {
	if (activeParticleCount >= maxParticles) return;

	size_t idx = activeParticleCount;
	positions[idx] = glm::vec4(position.x, position.y, position.z, rotation);
	velocities[idx] = velocity;
	colors[idx] = color;
	lives[idx] = lifeTime;


	activeParticleCount++;
}

void ParticleSystemNode::Process() {
	Object2D::Process();

	float deltaTime = Globals::GetGlobals().GetDeltaTime();

	for (size_t i = 0; i < activeParticleCount; ) {
		lives[i] -= deltaTime;

		if (lives[i] <= 0.0f) {
			size_t lastIdx = activeParticleCount - 1;
			positions[i] = positions[lastIdx];
			velocities[i] = velocities[lastIdx];
			colors[i] = colors[lastIdx];
			lives[i] = lives[lastIdx];
			activeParticleCount--;
		}
		else {
			velocities[i].y -= gravity * deltaTime;
			velocities[i] -= velocities[i] * damping * deltaTime;
			positions[i] += glm::vec4(velocities[i] * deltaTime, 0.0f);
			i++;
		}
	}
}

void ParticleSystemNode::Draw(std::shared_ptr<Shader> sh) {
	if (!TestDraw() || activeParticleCount == 0) return;
	if (sh == nullptr) sh = shader;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	sh->Use();

	glm::vec3 scale = GetTransform().GetScale();
	glm::vec2 finalScale = glm::vec2(scale.x, scale.y);

	if (GetSprite()) {
		glm::vec2 spriteSize = GetSprite()->GetSize();
		finalScale.x *= spriteSize.x;
		finalScale.y *= spriteSize.y;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GetSprite()->GetTexture(0).id);
		sh->SetInt("spriteTexture", 0);
	}
	sh->SetVec2("particleScale", finalScale);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Pos);
	glBufferSubData(GL_ARRAY_BUFFER, 0, activeParticleCount * sizeof(glm::vec4), positions.data());
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Col);
	glBufferSubData(GL_ARRAY_BUFFER, 0, activeParticleCount * sizeof(glm::vec4), colors.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(VAO);
	PROFILER_ADD_DRAW_CALL(2);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, activeParticleCount);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}