#version 440 core

in vec3 vPosition;

out vec4 fragColor;

void main()
{
    fragColor = vec4(vPosition, 1.0);
}
