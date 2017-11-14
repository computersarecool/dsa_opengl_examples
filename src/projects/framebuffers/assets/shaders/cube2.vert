#version 440 core

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

in vec3 aPosition;
in vec2 aUv;

out vec3 vPosition;
out vec2 vUv;

void main()
{
    vPosition = aPosition;
    vUv = aUv;
    gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
}
