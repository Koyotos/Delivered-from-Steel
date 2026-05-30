#version 420
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 M;
uniform mat4 VP;

out vec2 TexCoords; 

void main() {
    TexCoords = aUV; 
    gl_Position = VP * M * vec4(aPos, 0.0, 1.0);
}