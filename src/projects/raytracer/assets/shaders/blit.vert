#version 440 core

void main()
{
    vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.5, 1.0),
                       vec4( 1.0, -1.0, 0.5, 1.0),
                       vec4(-1.0,  1.0, 0.5, 1.0),
                       vec4( 1.0,  1.0, 0.5, 1.0));

    gl_Position = vertices[gl_VertexID];
}
