#version 440 core

uniform mat4 u_model_view_matrix;
uniform mat4 u_projection_matrix;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

layout (location = 0) out vec2 v_uv;

void main()
{
    v_uv = a_uv;
    gl_Position = u_projection_matrix * u_model_view_matrix * vec4(a_position, 1.0);
}
