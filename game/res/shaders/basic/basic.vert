#version 420
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords; 

out vec2 TexCoord;

uniform mat4 M;
uniform mat4 VP;

void main() {
  gl_Position =  VP * M * vec4(vertexPosition,1);
  TexCoord = texCoords;
}