#version 420

layout(location = 0) in vec3 position;
layout(location = 4) in mat4 instanceMatrix;

out vec3 WorldPos;

void main() {
    vec4 worldPos = instanceMatrix * vec4(position, 1.0);
    WorldPos = worldPos.xyz;
    gl_Position = worldPos;
}