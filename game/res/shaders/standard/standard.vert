#version 420

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords; 
layout(location = 3) in mat4 instanceMatrix;

layout(location = 0) out vec2 UV;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 FragPos;

uniform mat4 VP;

void main() {
    // Compute world-space position
    vec4 worldPos = instanceMatrix * vec4(vertexPosition, 1.0);
    FragPos = worldPos.xyz;

    // Transform normal into world space
    Normal = normalize(mat3(transpose(inverse(instanceMatrix))) * normals);

    UV = textCoords;

    // Project into clip space
    gl_Position = VP * worldPos;
}