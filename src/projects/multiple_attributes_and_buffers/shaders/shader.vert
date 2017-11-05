#version 440 core

in vec3 pos;
in vec3 color;

out VS_OUT
{
    vec3 projectionPos;
	vec3 color;
} vs_out;

void main()
{
    gl_Position = vec4(pos, 1.0);
    vs_out.projectionPos = gl_Position.xyz;
	vs_out.color = color;
}
