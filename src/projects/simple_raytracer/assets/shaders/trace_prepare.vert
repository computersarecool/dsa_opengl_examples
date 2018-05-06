#version 440 core

out VS_OUT
{
    vec3 ray_origin;
    vec3 ray_direction;
} vs_out;

uniform vec3 ray_origin;
uniform mat4 ray_lookat;

void main()
{
    vec4 vertices[4] = {
        {-1.0, -1.0, 1.0, 1.0},
        { 1.0, -1.0, 1.0, 1.0},
        {-1.0,  1.0, 1.0, 1.0},
        { 1.0,  1.0, 1.0, 1.0}
    };

    vec4 position = vertices[gl_VertexID];
    gl_Position = position;

    vec3 ray_direction = position.xyz - ray_origin;

    // The intersection tests expect direction to be normalized
    vs_out.ray_direction = normalize((ray_lookat * vec4(ray_direction, 0.0)).xyz);
    vs_out.ray_origin = ray_origin;
}
