#version 440 core

void main()
{
    vec4 vertices[4] = {
        {-1.0, -1.0, 1.0, 1.0},
        { 1.0, -1.0, 1.0, 1.0},
        {-1.0,  1.0, 1.0, 1.0},
        { 1.0,  1.0, 1.0, 1.0}
    };

    gl_Position = vertices[gl_VertexID];
}
