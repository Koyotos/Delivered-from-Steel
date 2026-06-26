#version 420

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;
layout(location = 3) in mat3 TBN;

layout(binding = 0) uniform sampler2D material_diffuse;
layout(binding = 1) uniform sampler2D material_specular;
layout(binding = 2) uniform sampler2D material_normal;

layout(binding = 9)  uniform sampler2DArray shadowMaps2D;
layout(binding = 10) uniform samplerCubeArray shadowCubemaps;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

struct Light {
    int  type;           // 0 = dir, 1 = point, 2 = spot
    vec3 colorDiffuse;
    vec3 colorSpecular;
    vec3 colorAmbient;
    vec3 data1;          // dir: direction; point/spot: position
    vec3 data2;          // spot: direction; point: attenuation
    vec3 data3;          // attenuation spot
    float data4;         // spot inner angle (radians)
};

uniform Light lights[20];
uniform int lightsNum;

uniform mat4 lightSpaceMatrices[20]; // for dir/spot
uniform float farPlanes[20];          // for point
uniform int shadowMapIndex2D[20];   // map light -> 2D layer
uniform int shadowMapIndexCube[20]; // map light -> cube layer

uniform vec3 viewPos;
uniform int dirSpotCount;

vec3 pointLightBrightness = vec3(0.0f);

vec3 matDiffuse  = vec3(0.0);
vec3 matSpecular = vec3(0.0);

const vec3 sampleOffsetDirections[20] = vec3[](
    vec3( 1,  1,  1), vec3(-1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1),
    vec3( 1,  1, -1), vec3(-1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1),
    vec3( 1,  0,  0), vec3(-1,  0,  0),
    vec3( 0,  1,  0), vec3( 0, -1,  0),
    vec3( 0,  0,  1), vec3( 0,  0, -1),
    vec3( 1,  1,  0), vec3(-1,  1,  0),
    vec3( 1, -1,  0), vec3(-1, -1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1)
);

const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2( 0.94558609, -0.76890725),
    vec2(-0.09418410, -0.92938870),
    vec2( 0.34495938,  0.29387760),
    vec2(-0.91588581,  0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543,  0.27676845),
    vec2( 0.97484398,  0.75648379),
    vec2( 0.44323325, -0.97511554),
    vec2( 0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2( 0.79197514,  0.19090188),
    vec2(-0.24188840,  0.99706507),
    vec2(-0.81409955,  0.91437590),
    vec2( 0.19984126,  0.78641367),
    vec2( 0.14383161, -0.14100790)
);

float Shadow2D(int i, vec3 normal, vec3 lightDir) {
    vec4 fragPosLightSpace = lightSpaceMatrices[i] * vec4(FragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.z < 0.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMaps2D, 0));
    float radius = texelSize.x * (0.75 + projCoords.z * 0.75);

    float shadow = 0.0;
    int samples = 16;

    for (int j = 0; j < samples; j++) {
        vec2 uv = projCoords.xy + poissonDisk[j] * radius;
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
            continue;

        float pcfDepth = texture(shadowMaps2D, vec3(uv, i)).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }

    return shadow / float(samples);
}


float ShadowCube(int i, vec3 normal) {
    // i is global light index (dir/spot first, then point)
    int pointIndex = i - dirSpotCount;   // 0..countPoint-1

    vec3 lightPos = lights[i].data1;
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);

    float farPlane = farPlanes[pointIndex];
    if (currentDepth > farPlane)
        return 0.0;

    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.002);

    float shadow = 0.0;
    int samples = 10;

    float diskRadius = (0.5 + currentDepth / farPlane) * 0.02;
    vec3 baseDir = normalize(fragToLight);

    for (int s = 0; s < samples; ++s) {
        vec3 offsetDir = sampleOffsetDirections[s] * diskRadius;
        vec3 sampleDir = baseDir + offsetDir;

        // Use pointIndex as cube-array layer
        float closestDepth = texture(shadowCubemaps, vec4(sampleDir, pointIndex)).r;
        closestDepth *= farPlane;

        shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    return shadow / float(samples);
}

vec3 DirectionalLight(int i, Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(-light.data1);

    vec3 ambient  = light.colorAmbient  * matDiffuse;

    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float diff = dot(normal, lightDir);
    vec3 diffuse  = light.colorDiffuse  * abs(diff) * matDiffuse;
    vec3 specular = light.colorSpecular * spec * matSpecular;
    
    float frontBlockMul = 0.0f;
    float backBlockMul = 0.0f;
    float ambientFaddingStart = -4.75;
    float ambientFaddingEnd = -6.25;
    float faddingStrengh = 0.2f;
    float lengthAmbient = ambientFaddingStart - ambientFaddingEnd;
    if (FragPos.z > ambientFaddingEnd){
        frontBlockMul = min((FragPos.z - ambientFaddingEnd) / lengthAmbient * faddingStrengh, faddingStrengh);
    }
    if (FragPos.z < ambientFaddingStart){
        backBlockMul = min((ambientFaddingStart - FragPos.z) / lengthAmbient * faddingStrengh, faddingStrengh);
    }
    vec3 shadowAmbient = diffuse * frontBlockMul;
    vec3 noShadowAmbient = diffuse * backBlockMul;

    if(diff <= 0.0)
        return ambient + shadowAmbient + diffuse * 0.2;

    float shadow = Shadow2D(i, normal, lightDir);


    float shadowFactor = 1.0 - shadow;
    vec3 lighting = (diffuse + specular) * shadowFactor;

    return ambient + lighting + (shadowAmbient + diffuse * 0.2) * shadow - noShadowAmbient * shadowFactor;
}

vec3 PointLight(int i, Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(light.data1 - FragPos);

    vec2 lightPosXY = light.data1.xy;
    vec2 fragPosXY = FragPos.xy;

    float brightIntens = FragPos.z - light.data1.z + 1.0;
    if (brightIntens < 1.0)
        brightIntens = 1.0;
    brightIntens = 1/(brightIntens*brightIntens);
    float distXY = length(lightPosXY - fragPosXY);
    float attenuationXY = 1.0 / (light.data2.x + light.data2.y * distXY + light.data2.z * distXY * distXY);

    pointLightBrightness += light.colorDiffuse * attenuationXY * brightIntens /4.0f;

    float diff = dot(normal, lightDir);
    if(diff <= 0.0)
        return light.colorAmbient * matDiffuse;
    diff = max(diff, 0.0);

    float shadow = ShadowCube(i, normal);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float dist = length(light.data1 - FragPos);
    float attenuation = 1.0 / (light.data2.x + light.data2.y * dist + light.data2.z * dist * dist);

    vec3 ambient  = light.colorAmbient  * matDiffuse;
    vec3 diffuse  = light.colorDiffuse  * diff * matDiffuse;
    vec3 specular = light.colorSpecular * spec * matSpecular;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    float shadowFactor = 1.0 - shadow;
    vec3 lighting = (diffuse + specular) * shadowFactor;
    vec3 ambientShadowed = ambient * mix(0.2, 1.0, shadowFactor);

    return ambientShadowed + lighting;
}

vec3 SpotLight(int i, Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightPos = light.data1;
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = dot(normal, lightDir);
    if(diff <= 0.0)
        return light.colorAmbient * matDiffuse;
    diff = max(diff, 0.0);

    float shadow = Shadow2D(i, normal, lightDir);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float dist = length(lightPos - FragPos);
    float attenuation = 1.0 / (light.data3.x + light.data3.y * dist + light.data3.z * dist * dist);

    float innerAngle = light.data4 * 0.5;
    float outerAngle = innerAngle + radians(5.0);

    float innerCutoff = cos(innerAngle);
    float outerCutoff = cos(outerAngle);

    float theta = dot(lightDir, normalize(-light.data2));
    float epsilon = innerCutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

    vec3 ambient  = light.colorAmbient  * matDiffuse;
    vec3 diffuse  = light.colorDiffuse  * diff * matDiffuse;
    vec3 specular = light.colorSpecular * spec * matSpecular;

    ambient  *= attenuation;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    float shadowFactor = 1.0 - shadow;
    vec3 lighting = (diffuse + specular) * shadowFactor;
    vec3 ambientShadowed = ambient * mix(0.2, 1.0, shadowFactor);

    return ambientShadowed + lighting;
}

vec3 ApplyLight(int i, Light l, vec3 normal, vec3 viewDir) {
    if (l.type == 0) return DirectionalLight(i, l, normal, viewDir);
    if (l.type == 1) return PointLight(i, l, normal, viewDir);
    if (l.type == 2) return SpotLight(i, l, normal, viewDir);
    return vec3(0.0);
}

void main() {
    
    // Normal mapping
    vec3 nMap = texture(material_normal, UV).rgb;
    nMap = nMap * 2.0 - 1.0;
    vec3 norm = normalize(TBN * nMap);

    vec3 viewDir = normalize(viewPos - FragPos);

    matDiffuse  = texture(material_diffuse,  UV).rgb;
    matSpecular = texture(material_specular, UV).rgb;

    vec3 result = vec3(0.0);
    for (int i = 0; i < lightsNum; i++) {
        result += ApplyLight(i, lights[i], norm, viewDir);
    }

    FragColor = vec4(result, 1.0);

    BrightColor = vec4(pointLightBrightness, 1.0);
}