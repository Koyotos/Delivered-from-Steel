#version 420

in vec2 TexCoords;

uniform sampler2D spriteTexture;
uniform vec3 tint;
uniform float alpha;

out vec4 FragColor;

void main()
{
    vec4 tex = texture(spriteTexture, TexCoords);

    tex.rgb *= tint;
    tex.a *= alpha;

    if (tex.a < 0.1)
        discard;

    FragColor = tex;
}