#version 440 core

uniform float offsetX;
uniform float offsetY;
uniform float scale;
uniform int imageIndex;

out VS_OUT
{
    flat int imageIndex;
    vec2 uv;
} vs_out;

void main()
{
  const vec2[4] positions = vec2[4](vec2(-0.5, -0.5),
                                   vec2(0.5, -0.5),
                                   vec2(-0.5, 0.5),
                                   vec2(0.5, 0.5));

  vs_out.uv = positions[gl_VertexID].xy + vec2(0.5);
  vec2 position = positions[gl_VertexID].xy + vec2(offsetX, offsetY);
  position.x *= scale;
  position.y *= scale;
  gl_Position = vec4(position, 0.0, 1.0);
  vs_out.imageIndex = imageIndex;
}
