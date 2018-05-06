#version 440 core

in vec3 position;
in vec3 color;

out VS_OUT
{
	vec3 color;
} vs_out;

void main()
{
    gl_Position = vec4(position, 1.0);
	vs_out.color = color;
}
