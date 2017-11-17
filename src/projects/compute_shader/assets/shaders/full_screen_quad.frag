#version 440 core

layout (binding = 0) uniform sampler2D input_image;

layout (location = 0) out vec4 fragColor;

void main()
{
    vec4 color = texelFetch(input_image, ivec2(gl_FragCoord.xy), 0).rgba;
    fragColor = color;
}
