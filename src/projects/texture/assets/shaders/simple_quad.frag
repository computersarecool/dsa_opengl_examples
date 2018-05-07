#version 440 core

in VS_OUT
{
    vec2 uv;
} fs_in;

uniform sampler2D image_sampler;

out vec4 frag_color;

void main()
{
    frag_color= texture(image_sampler, fs_in.uv);
}
