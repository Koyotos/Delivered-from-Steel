#version 420
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 M;
uniform mat4 VP;

out vec2 TexCoords; 

uniform vec2 spriteSize;

void main() {
    vec2 pos = aPos;
    TexCoords = aUV; 
    pos.x = aPos.x-spriteSize.x/2;
    pos.y = aPos.y-spriteSize.y/2;
    gl_Position = VP * M * vec4(pos, 0.0, 1.0);
}