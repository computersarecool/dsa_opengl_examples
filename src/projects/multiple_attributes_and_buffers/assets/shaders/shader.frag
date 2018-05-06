#version 440 core

in VS_OUT
{
	vec3 color;
} fs_in;

out vec4 frag_color;

void main()
{
    frag_color = vec4(fs_in.color, 1.0);
}
