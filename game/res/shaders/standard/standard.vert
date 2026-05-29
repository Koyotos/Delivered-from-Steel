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
    // World-space position
    vec4 worldPos = instanceMatrix * vec4(vertexPosition, 1.0);
    FragPos = worldPos.xyz;

    // Normal matrix from instanceMatrix
    mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix)));

    // World-space normal
    vec3 N = normalize(normalMatrix * normal);
    Normal = N;

    // World-space tangent
    vec3 T = normalize(normalMatrix * tangent);
    // Orthonormalize T against N
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    UV = textCoords;

    gl_Position = VP * worldPos;
}
