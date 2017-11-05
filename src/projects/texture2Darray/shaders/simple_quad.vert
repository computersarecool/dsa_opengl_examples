#version 440 core

uniform float offsetX;
uniform float offsetY;
uniform float scale;
uniform float index;

out VS_OUT
{
  flat int banana;
  vec2 tc;
} vs_out;

void main()
{
  const vec2[4] position = vec2[4](vec2(-0.5, -0.5),
                                   vec2(0.5, -0.5),
                                   vec2(-0.5, 0.5),
                                   vec2(0.5, 0.5));

  vs_out.tc = position[gl_VertexID].xy + vec2(0.5);
  vec2 pos = position[gl_VertexID].xy + vec2(offsetX, offsetY);
  pos.x *= scale;
  pos.y *= scale;
  gl_Position = vec4(pos, 0.0, 1.0);
  vs_out.banana = int(index);
}
