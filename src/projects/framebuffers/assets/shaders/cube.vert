#version 440 core

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

in vec3 aPosition;

out vec3 vPosition;

void main()
{
    vPosition = aPosition;
    gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
}
