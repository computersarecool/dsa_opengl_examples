#version 440 core

uniform float u_offset_x;
uniform float u_offset_y;
uniform float u_scale;
uniform int u_image_index;

out VS_OUT
{
    flat int image_index;
    vec2 uv;
} vs_out;

void main()
{
  const vec2[4] positions = {
    {-0.5, -0.5},
    {0.5, -0.5},
    {-0.5, 0.5},
    {0.5, 0.5}
  };

  // Flip the y coordinate
  vs_out.uv = vec2(0, 1.0) - positions[gl_VertexID].xy + 0.5;

  vec2 position = positions[gl_VertexID].xy + vec2(u_offset_x, u_offset_y);
  position *= vec2(u_scale);
  gl_Position = vec4(position, 0.0, 1.0);

  vs_out.image_index = u_image_index;
}
