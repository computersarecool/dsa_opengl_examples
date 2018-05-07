#version 440 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

out VS_OUT
{
    vec3 normal;
} vsOut;

void main()
{
    vsOut.normal = a_normal;
    gl_Position = vec4(a_position, 1.0);
}
