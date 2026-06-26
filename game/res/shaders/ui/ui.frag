#version 420

in vec2 TexCoords;

uniform sampler2D spriteTexture;
uniform vec3 tint;
uniform float alpha;

out vec4 FragColor;
out vec4 BrightColor;

void main()
{
    vec4 tex = texture(spriteTexture, TexCoords);

    tex.rgb *= tint;
    tex.a *= alpha;

    if (tex.a < 0.1)
        discard;

    FragColor = tex;
    BrightColor = vec4(0.0);
}