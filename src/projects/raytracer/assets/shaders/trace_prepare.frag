#version 440 core

in VS_OUT
{
    vec3 ray_origin;
    vec3 ray_direction;
} fs_in;

layout (location = 0) out vec3 out_composite_color;
layout (location = 1) out vec3 out_origin;
layout (location = 2) out vec3 out_direction;
layout (location = 3) out vec3 out_reflected_color;

void main()
{
    out_composite_color = vec3(0.0);
    out_origin = fs_in.ray_origin;
    out_direction = fs_in.ray_direction;
    out_reflected_color = vec3(1.0);
}
