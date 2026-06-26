#version 420
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;
uniform vec3 camPos;

uniform sampler2D depthBuffer;
uniform sampler2DArray shadowMaps2D;
uniform mat4 invProjection;
uniform mat4 invView;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform mat4 sunMatrix;
uniform float saturationValue;

uniform mat4 projection;
uniform mat4 view;

uniform bool godRays;
uniform bool sunExists;
uniform bool saturationControl;

vec3 reconstructWorldPos(vec2 uv, float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clip = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 view = invProjection * clip;
    view /= view.w;

    vec4 world = invView * view;
    return world.xyz;
}

vec3 computeVolumetric(vec2 uv) {
    float depth = texture(depthBuffer, uv).r;
    vec3 worldPos = reconstructWorldPos(uv, depth);

    vec3 rayDir = normalize(worldPos - camPos);

    float tMax = length(worldPos - camPos);
    float stepSize = tMax / 16.0;

    vec3 pos = camPos;
    vec3 result = vec3(0.0);

    for(int i = 0; i < 16; i++) {
        pos += rayDir * stepSize;

        vec4 viewPos = view * vec4(pos, 1.0);
        vec4 clipPos = projection * viewPos;
        vec3 ndc = clipPos.xyz / clipPos.w;
        vec2 sampleUV = ndc.xy * 0.5 + 0.5;
        
        float sceneDepth = texture(depthBuffer, sampleUV).r;
        if (viewPos.z > sceneDepth) continue; 

        vec4 lightSpace = sunMatrix * vec4(pos, 1.0);
        vec3 proj = lightSpace.xyz / lightSpace.w;
        proj = proj * 0.5 + 0.5;

        float shadow = texture(shadowMaps2D, vec3(proj.xy, 0)).r;
        float current = proj.z;

        float visibility = current < shadow ? 1.0 : 0.0;

        // Henyey-Greenstein approx (optional)
        float scattering = 0.1;

        result += visibility * lightColor * scattering;
    }

    return result / 16.0;
}

vec4 AdjustSaturation(vec3 color) {
    float L = dot(color, vec3(0.299, 0.587, 0.114));
    return vec4(mix(vec3(L), color, saturationValue),1.0);
}

void main() {
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(bloom) {
        vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
        hdrColor += bloomColor;
    }
    if(godRays && sunExists) {
        vec3 volumetric = computeVolumetric(TexCoords);

        hdrColor += volumetric;
    }
    

    hdrColor = (hdrColor - 0.5) * 1.2 + 0.5;

    FragColor = vec4(hdrColor, 1.0);


    if(saturationControl) {
        FragColor = AdjustSaturation(FragColor.rgb);
    }
    
}