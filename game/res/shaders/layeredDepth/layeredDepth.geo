#version 420
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];
uniform int lightType;

void main()
{
    int faces = (lightType == 1) ? 6 : 1;
    for(int face = 0; face < faces; face++) {
        gl_Layer = face;

        for(int i = 0; i < 3; i++) {
            gl_Position = shadowMatrices[face] * gl_in[i].gl_Position;
            EmitVertex();
        }

        EndPrimitive();
    }
}