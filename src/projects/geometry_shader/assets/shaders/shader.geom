#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];

out vec4 gPosition;

void main()
{
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        gPosition = vPosition[i];
        EmitVertex();
    }
    EndPrimitive();
}
