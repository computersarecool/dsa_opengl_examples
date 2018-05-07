#version 440 core

uniform mat4 u_projection_matrix;
uniform float u_x_offset;
uniform float u_y_offset;

vec4 starting_position = vec4(0.0, 0.0, -5.0, 1.0);

void main()
{
    starting_position.xy += vec2(u_x_offset, u_y_offset);
    gl_Position = u_projection_matrix * starting_position;
    gl_PointSize = 15.0;
}
