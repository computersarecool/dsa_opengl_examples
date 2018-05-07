#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec4 v_position[];

layout (location = 0) out vec4 g_position;

void main()
{
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        g_position = v_position[i];
        EmitVertex();
    }
    EndPrimitive();
}
