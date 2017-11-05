#version 440 core

out vec4 fragColor;

layout (binding = 1) uniform sampler2D uTexColor;

in TES_OUT
{
    vec2 uv;
} fsIn;

void main()
{
    fragColor = texture(uTexColor, fsIn.uv);
}
