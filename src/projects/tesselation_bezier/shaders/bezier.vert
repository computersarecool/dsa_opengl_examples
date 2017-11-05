#version 440 core

const float quadHalfExtent = 0.5;

uniform mat4 uModelViewMatrix;

in vec4 vPosition;

void main()
{
    gl_Position = uModelViewMatrix * vPosition;
}
