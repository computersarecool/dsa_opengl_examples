#version 440 core

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

in vec4 aPosition;
in vec3 aColor;

out vec3 vColor;

void main()
{
    vColor = aColor;
    gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;
}
