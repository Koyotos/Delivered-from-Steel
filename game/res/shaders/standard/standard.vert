#version 420

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textCoords; 
layout(location = 3) in vec3 tangent; 
layout(location = 4) in mat4 instanceMatrix;

layout(location = 0) out vec2 UV;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 FragPos;
layout(location = 3) out mat3 TBN;

uniform mat4 VP;

void main() {
    // Compute world-space position
    vec4 worldPos = instanceMatrix * vec4(vertexPosition, 1.0);
    FragPos = worldPos.xyz;

    // Transform normal into world space
    Normal = normalize(mat3(transpose(inverse(instanceMatrix))) * normal);

    UV = textCoords;

    // Project into clip space
    gl_Position = VP * worldPos;

    vec3 T = normalize(vec3(instanceMatrix * vec4(tangent,   0.0)));
    vec3 N = normalize(vec3(instanceMatrix * vec4(normal,    0.0)));

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
}