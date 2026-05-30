#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 VP;
uniform mat4 M;

void main()
{
    gl_Position = VP * M * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}  