#version 420
layout(location = 0) in vec4 fragmentColor;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 FragPos;

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

uniform Material material;
uniform Light lights[20];
uniform vec3 viewPos;
uniform int lightsNum;

vec3 DirectionalLight(Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(-light.data1);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128);

    vec3 ambient  = light.colorAmbient  * vec3(texture(material.diffuse, UV));
    vec3 diffuse  = light.colorDiffuse  * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.colorSpecular * spec * vec3(texture(material.specular, UV));
    return (ambient + diffuse + specular);
}

vec3 PointLight(Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(light.data1 - FragPos);

    // Calculate lights
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128);
    float dist = length(light.data1 - FragPos);
    float attenuation = 1.0 / (light.data2.x + light.data2.y * dist + light.data2.z * (dist * dist));    

    // Combine
    vec3 ambient  = light.colorAmbient  * vec3(texture(material.diffuse, UV));
    vec3 diffuse  = light.colorDiffuse  * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.colorSpecular * spec * vec3(texture(material.specular, UV));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 SpotLight(Light light, vec3 normal, vec3 viewDirection) 
{
    vec3 lightDir = normalize(light.data1 - FragPos);

    // Basic lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128);

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
    vec3 ambient  = light.colorAmbient  * vec3(texture(material.diffuse, UV));
    vec3 diffuse  = light.colorDiffuse  * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.colorSpecular * spec * vec3(texture(material.specular, UV));

    ambient  *= attenuation;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}


vec3 ApplyLight(Light l, vec3 normal, vec3 viewDir) {
    switch(l.type) {
        case 0: return DirectionalLight(l, normal, viewDir);
        case 1: return PointLight(l, normal, viewDir);
        case 2: return SpotLight(l, normal, viewDir);
        default: break;
    }
    return vec3(0.0, 0.0, 0.0);
}

void main()
{  
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0,0.0,0.0);
    for(int i = 0; i <= lightsNum; i++) {
        result += ApplyLight(lights[i], norm, viewDir);
    }  
    
    FragColor = vec4(result, 1.0);
} 