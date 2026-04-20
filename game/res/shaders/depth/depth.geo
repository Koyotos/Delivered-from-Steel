#version 420
in vec3 worldPos;
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];
uniform bool point;

out vec4 FragPos;

void main() {
    int faceCount = point ? 6 : 1;
    for(int face = 0; face < faceCount; face++) {
        if (point) {
            gl_Layer = face;
        }
        for (int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}