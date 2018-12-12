#version 440 core

layout (location = 0) out vec4 frag_color;

uniform float time;

void main()
{
    float red_amount = (sin(time * 4.0) + 1.0) / 2.0;
    frag_color = vec4(red_amount, 0.0, 0.0, 1.0);
}
