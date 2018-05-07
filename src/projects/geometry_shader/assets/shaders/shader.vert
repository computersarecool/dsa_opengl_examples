#version 440 core

uniform mat4 u_model_view_matrix;

layout (location = 0) in vec4 a_position;

layout (location = 0) out vec4 v_position;

void main()
{
    gl_Position = u_model_view_matrix * a_position;
    v_position = gl_Position;
}
