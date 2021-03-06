#version 440 core

uniform mat4 u_model_view_matrix;
uniform mat4 u_projection_matrix;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

layout (location = 0) out vec3 v_position;

void main()
{
    v_position = a_position;
    gl_Position = u_projection_matrix * u_model_view_matrix * vec4(a_position, 1.0);
}
