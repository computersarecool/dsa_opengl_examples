#version 440 core

in VS_OUT
{
    flat int image_index;
    vec2 uv;
} fs_in;

uniform sampler2DArray image_sampler;

layout (location = 0) out vec4 frag_color;

void main()
{
  frag_color = texture(image_sampler, vec3(fs_in.uv, float(fs_in.image_index)));
}
