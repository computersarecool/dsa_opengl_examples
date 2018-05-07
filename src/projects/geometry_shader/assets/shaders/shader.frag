#version 440 core

layout (location = 0)in vec4 g_position;

layout (location = 0) out vec4 frag_color;

void main()
{
    frag_color = g_position;
}
