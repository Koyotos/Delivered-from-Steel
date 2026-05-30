#version 420 
in vec2 TexCoords;

uniform sampler2D spriteTexture;

out vec4 FragColor;

void main() { 
    FragColor = texture(spriteTexture, TexCoords);
    if(FragColor.a < 0.1) {
        discard;
    }
}