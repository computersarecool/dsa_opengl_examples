#version 440 core

vec4 starting_position = vec4(0.0, 0.0, -5.0, 1.0);

uniform mat4 uProjectionMatrix;
uniform float xOffset;
uniform float yOffset;

void main()
{
    starting_position.xy += vec2(xOffset, yOffset);
    gl_Position = uProjectionMatrix * starting_position;
    gl_PointSize = 15.0;
}
