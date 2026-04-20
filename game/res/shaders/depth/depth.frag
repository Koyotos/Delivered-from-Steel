#version 420

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;
uniform bool point;

void main()
{
    if(point) {
        float lightDistance = length(FragPos.xyz - lightPos);
        lightDistance /= farPlane;
        gl_FragDepth = lightDistance;
    } else {
        gl_FragDepth = gl_FragCoord.z;
    }
}