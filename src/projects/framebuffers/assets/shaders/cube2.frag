#version 440 core

uniform sampler2D fboTexture;

in vec3 vPosition;
in vec2 vUv;

out vec4 fragColor;

void main()
{

    fragColor = texture(fboTexture, vUv) + vec4(0.5);
}
