#version 420 
in vec2 TexCoords;

uniform sampler2D spriteTexture;

uniform sampler2DArray shadowMaps2D;
uniform mat4 sunMatrix;

in vec3 FragPos;
out vec4 FragColor;
out vec4 BrightColor;

float ShadowCalculation(vec3 fragPos) {
    vec4 fragPosLightSpace = sunMatrix * vec4(fragPos, 1.0);
    
    vec3 projCoords = fragPosLightSpace.xyz;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture(shadowMaps2D, vec3(projCoords.xy, 0)).r;
    
    float currentDepth = projCoords.z;
    
    return (currentDepth > closestDepth) ? 1.0 : 0.0;
}

void main() { 
    FragColor = texture(spriteTexture, TexCoords);
    if(FragColor.a < 0.1) {
        BrightColor = vec4(0.0);
        discard;
    }
    float shadow = 0.8 - ShadowCalculation(FragPos) * 0.3f;
    vec3 color = FragColor.rgb;
    FragColor = vec4(color * shadow, 1.0);
    
    BrightColor = vec4(0.0);
}