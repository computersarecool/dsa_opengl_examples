#version 440 core

void main()
{
    vec4 vertices[4] = {
        {-1.0, -1.0, 0.5, 1.0},
        { 1.0, -1.0, 0.5, 1.0},
        {-1.0,  1.0, 0.5, 1.0},
        { 1.0,  1.0, 0.5, 1.0}
    };

    vec4 position = vertices[gl_VertexID];

    gl_Position = position;
}

