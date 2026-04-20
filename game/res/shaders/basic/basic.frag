#version 420

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D txt;

void main()
{
    FragColor = texture(txt, TexCoord);
} 