#version 440 core

layout (binding = 0) uniform sampler2D fbo_texture;

layout (location = 0) in vec2 v_uv;

layout (location = 0) out vec4 frag_color;

void main()
{

    frag_color = texture(fbo_texture, v_uv) + vec4(0.5);
}
