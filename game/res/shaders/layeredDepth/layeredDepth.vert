#version 420

layout(location = 0) in vec3 position;
layout(location = 3) in mat4 instanceMatrix;

uniform mat4 M;

out vec3 WorldPos;

void main()
{
    WorldPos = vec3(instanceMatrix * vec4(position, 1.0));
}