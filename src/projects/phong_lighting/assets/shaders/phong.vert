#version 440 core

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
uniform mat3 u_normal_matrix;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out VS_OUT {
    vec3 world_space_position;
    vec3 world_space_normal;
    vec2 uv;
} v_vert;

void main()
{
    vec4 world_space_position = u_model_matrix * vec4(a_position, 1.0);
    gl_Position = u_projection_matrix * u_view_matrix * world_space_position;

    v_vert.world_space_position = world_space_position.xyz;
    v_vert.world_space_normal = normalize(u_normal_matrix * a_normal);
    v_vert.uv = a_uv;
}
