#version 440 core

void main()
{
    const vec4 vertices[] = {
        {-1.0, -1.0, 0.0, 1.0},
        {1.0, -1.0, 0.0, 1.0},
        {-1.0,  1.0, 0.0, 1.0},
        {1.0,  1.0, 0.0, 1.0}
    };

    gl_Position = vertices[gl_VertexID];
}
