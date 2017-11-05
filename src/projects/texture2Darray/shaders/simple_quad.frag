#version 440 core

in VS_OUT
{
  flat int banana;
  vec2 tc;
} fs_in;

uniform sampler2DArray ourTexture;

out vec4 FragColor;

void main()
{
  FragColor = texture(ourTexture, vec3(fs_in.tc, float(fs_in.banana)));
}
