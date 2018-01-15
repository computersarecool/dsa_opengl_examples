#version 440 core

in VS_OUT
{
    vec2 uv;
} fs_in;

uniform sampler2D imageSampler;

out vec4 FragColor;

void main()
{
    FragColor = texture(imageSampler, fs_in.uv);
}
