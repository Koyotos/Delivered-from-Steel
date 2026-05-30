#version 420 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aInstanceData;
layout (location = 2) in vec4 aColor;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 VP;
uniform vec2 particleScale;

void main() {
    ParticleColor = aColor;
    TexCoords = aPos + vec2(0.5);
	
	float rotation = aInstanceData.w;
	vec3 offset = aInstanceData.xyz;
	
	float cosR = cos(rotation);
	float sinR = sin(rotation);
	
	vec2 rotatedPos;
	rotatedPos.x = aPos.x * cosR - aPos.y * sinR;
	rotatedPos.y = aPos.x * sinR + aPos.y * cosR;
	
    vec3 vertexPosition_worldspace = offset + vec3(rotatedPos * particleScale, 0.0);
    gl_Position = VP * vec4(vertexPosition_worldspace, 1.0);
}