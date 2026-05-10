#version 420
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;

uniform sampler2D depthBuffer;
uniform sampler2DArray shadowMaps2D;
uniform mat4 invProjection;
uniform mat4 invView;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform mat4 sunMatrix;

uniform bool godRays;
uniform bool sunExists;

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

    vec3 camPos = (invView * vec4(0,0,0,1)).xyz;
    vec3 rayDir = normalize(worldPos - camPos);

    float tMax = length(worldPos - camPos);
    float stepSize = tMax / 64.0;

    vec3 pos = camPos;
    vec3 result = vec3(0.0);

    for(int i = 0; i < 64; i++) {
        pos += rayDir * stepSize;

        // transform to light space
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

    return result / 64.0;
}

void main() {
    if(!godRays || !sunExists) {
        FragColor = vec4(texture(hdrBuffer, TexCoords).rgb, 1.0);
        return;
    }
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    vec3 volumetric = computeVolumetric(TexCoords);

    hdrColor += volumetric;

    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}