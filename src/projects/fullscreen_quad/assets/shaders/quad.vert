#version 440 core

const vec2[4] positions = {
    {-1.0, -1.0},
    {1.0, -1.0},
    {-1.0, 1.0},
    {1.0, 1.0}
};

void main()
{
    gl_Position = vec4(positions[gl_VertexID].xy, 0.0, 1.0);
}
