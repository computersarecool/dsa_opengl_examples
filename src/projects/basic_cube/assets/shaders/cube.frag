#version 440 core

in vec3 v_position;
out vec4 frag_color;

void main()
{
    frag_color = vec4(v_position, 1.0);
}
