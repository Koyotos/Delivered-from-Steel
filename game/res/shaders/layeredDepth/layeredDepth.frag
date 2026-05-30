#version 420

in vec3 WorldPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDistance = length(WorldPos - lightPos);

    // map [0, farPlane] → [0, 1]
    lightDistance = lightDistance / farPlane;

    gl_FragDepth = lightDistance;
}
