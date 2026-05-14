#version 420 core
in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D spriteTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(spriteTexture, TexCoords) * ParticleColor;
    if(FragColor.a < 0.1) {
        discard;
    }
}