#version 440 core

in VS_OUT
{
    flat int imageIndex;
    vec2 uv;
} fs_in;

uniform sampler2DArray imageSampler;

out vec4 FragColor;

void main()
{
  FragColor = texture(imageSampler, vec3(fs_in.uv, float(fs_in.imageIndex)));
}
