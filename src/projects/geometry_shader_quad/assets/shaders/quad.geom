#version 440 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 6) out;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

in vec3 vColor[4];

out GS_OUT
{
    flat vec3 color[4];
    vec2 uv;
} gsOut;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    gsOut.uv = vec2(0.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    gsOut.uv = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    gsOut.uv = vec2(1.0);
    EmitVertex();

    // Last vertex is provoking vertex
    gsOut.color[0] = vColor[1];
    gsOut.color[1] = vColor[0];
    gsOut.color[2] = vColor[2];
    gsOut.color[3] = vColor[3];
    EmitVertex();
    EndPrimitive();


    gl_Position = gl_in[0].gl_Position;
    gsOut.uv = vec2(0.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    gsOut.uv = vec2(1.0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    gsOut.uv = vec2(1.0, 0.0);
    EmitVertex();

    // Last vertex is provoking vertex
    gsOut.color[0] = vColor[1];
    gsOut.color[1] = vColor[0];
    gsOut.color[2] = vColor[2];
    gsOut.color[3] = vColor[3];
    EmitVertex();
    EndPrimitive();
}
