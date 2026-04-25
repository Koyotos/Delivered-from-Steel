#version 420
layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;

layout(location = 0) out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
};

struct Light {
    int type;
    vec3 colorDiffuse;
    vec3 colorSpecular;
    vec3 colorAmbient;
    vec3 data1;
    vec3 data2;
    vec3 data3;
    float data4;
};

vec3 matDiffuse;
vec3 matSpecular;

uniform Material material;
uniform Light lights[20];
uniform vec3 viewPos;
uniform int lightsNum;
uniform sampler2D shadowMaps2D[20];
uniform samplerCube shadowCubemaps[20];
uniform mat4 lightSpaceMatrices[20];
uniform float farPlanes[20];

float Shadow2D(int i, vec3 normal, vec3 lightDir) {
    vec4 fragPosLightSpace = lightSpaceMatrices[i] * vec4(FragPos, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps2D[i], 0);

    for(int x = -1; x <= 1; x++)
    for(int y = -1; y <= 1; y++) {
        float pcfDepth = texture(shadowMaps2D[i], projCoords.xy + vec2(x,y)*texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }

    shadow /= 9.0;

    return shadow;
}

float ShadowCube(int i) {
    vec3 fragToLight = FragPos - lights[i].data1;
    float currentDepth = length(fragToLight);

    if(currentDepth > farPlanes[i])
        return 0.0;

    float bias = 0.005;
    float closestDepth = texture(shadowCubemaps[i], fragToLight).r;
    closestDepth *= farPlanes[i];

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 DirectionalLight(int i, Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(-light.data1);

    float shadow = Shadow2D(i, normal, lightDir);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 ambient  = light.colorAmbient  * matDiffuse;
    vec3 diffuse  = light.colorDiffuse  * diff * matDiffuse;
    vec3 specular = light.colorSpecular * spec * matSpecular;

    float shadowFactor = mix(1.0, 0.0, shadow);
    vec3 lighting = (diffuse + specular) * shadowFactor;

    vec3 ambientShadowed = ambient * mix(0.2, 1.0, shadowFactor);

    return ambientShadowed + lighting;
}

vec3 PointLight(int i, Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(light.data1 - FragPos);

    float shadow = ShadowCube(i);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float dist = length(light.data1 - FragPos);
    float attenuation = 1.0 / (light.data2.x + light.data2.y * dist + light.data2.z * dist * dist);

    vec3 ambient  = light.colorAmbient  * matDiffuse;
    vec3 diffuse  = light.colorDiffuse  * diff * matDiffuse;
    vec3 specular = light.colorSpecular * spec * matSpecular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadowFactor = mix(1.0, 0.0, shadow);
    vec3 lighting = (diffuse + specular) * shadowFactor;
    vec3 ambientShadowed = ambient * mix(0.2, 1.0, shadowFactor);
    return ambientShadowed + lighting;
}

vec3 SpotLight(int i, Light light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDir = normalize(light.data1 - FragPos);

    float shadow = Shadow2D(i, normal, lightDir);

    // Basic lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float dist = length(light.data1 - FragPos);
    float attenuation = 1.0 / (light.data3.x + light.data3.y * dist + light.data3.z * dist * dist);

    // Spotlight cutoff
    float innerCutoff = light.data4;

    // Derive outer cutoff = inner + 5 degrees
    float outerCutoff = cos(acos(innerCutoff) + radians(5.0));

    float theta = dot(lightDir, normalize(-light.data2));
    float epsilon = innerCutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

    // Combine lighting
    vec3 ambient  = light.colorAmbient  * matDiffuse;
    vec3 diffuse  = light.colorDiffuse  * diff * vec3(matDiffuse);
    vec3 specular = light.colorSpecular * spec * vec3(matSpecular);

    ambient  *= attenuation;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    float shadowFactor = mix(1.0, 0.0, shadow);
    vec3 lighting = (diffuse + specular) * shadowFactor;
    vec3 ambientShadowed = ambient * mix(0.2, 1.0, shadowFactor);
    return ambientShadowed + lighting;
}

vec3 ApplyLight(int i, Light l, vec3 normal, vec3 viewDir) {
    switch(l.type) {
        case 0: return DirectionalLight(i, l, normal, viewDir);
        case 1: return PointLight(i, l, normal, viewDir);
        case 2: return SpotLight(i, l, normal, viewDir);
    }
    return vec3(0.0);
}

void main()
{  
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    matDiffuse = texture(material.diffuse, UV).rgb;
    matSpecular = texture(material.specular, UV).rgb;

    vec3 result = vec3(0.0,0.0,0.0);
    for(int i = 0; i < lightsNum; i++) {
        result += ApplyLight(i, lights[i], norm, viewDir);
    }
    
    FragColor = vec4(pow(result, vec3(1.0/2.2)), 1.0);
} 