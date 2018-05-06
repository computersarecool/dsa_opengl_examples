#version 440 core

layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D composite_color_texture;

void main()
{
    frag_color = texelFetch(composite_color_texture, ivec2(gl_FragCoord.xy), 0);
}
