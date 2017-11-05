#version 440 core

uniform mat4 uModelViewMatrix;

in vec4 aPosition;

out vec4 vPosition;

void main()
{
    gl_Position = uModelViewMatrix * aPosition;
    vPosition = gl_Position;
}
