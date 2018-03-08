#version 440 core

in vec4 gPosition;

out vec4 fragColor;

void main()
{
    fragColor = gPosition;
//    fragColor = gl_FragCoord;
}
