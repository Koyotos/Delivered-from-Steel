#version 330 core
in vec2 TexCoords;
out vec4 FragColor;
out vec4 BrightColor;
uniform sampler2D text;
uniform vec3 color;
uniform float alpha;
void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    FragColor = vec4(color, alpha) * sampled;
    BrightColor = vec4(0.0);
}