#version 420
layout(location = 0) in vec3 position;
layout(location = 3) in mat4 instanceMatrix;

uniform mat4 M;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = lightSpaceMatrix * instanceMatrix * vec4(position, 1.0);
}