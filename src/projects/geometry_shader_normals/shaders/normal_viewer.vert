#version 440 core

in vec3 aPosition;
in vec3 aNormal;

out VS_OUT
{
    vec3 normal;
} vsOut;

void main()
{
    vsOut.normal = aNormal;
    gl_Position = vec4(aPosition, 1.0);
}
