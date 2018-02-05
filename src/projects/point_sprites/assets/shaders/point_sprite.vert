#version 440 core

uniform mat4 uProjectionMatrix;
const vec4 starting_position = vec4(0.5, 0.5, 0.0, 1.0);

void main()
{
    // gl_Position = uProjectionMatrix * starting_position;
    gl_Position = starting_position;
    gl_PointSize = 35.0;
}
