#version 420

layout(location = 0) in vec3 position;

uniform mat4 M;

out vec3 WorldPos;

void main()
{
    WorldPos = vec3(M * vec4(position, 1.0));
}