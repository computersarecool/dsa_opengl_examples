#version 440 core

layout (location = 0) in vec3 v_position;
layout (location = 0) out vec4 frag_color;

void main()
{
    frag_color = vec4(v_position, 1.0);
}
