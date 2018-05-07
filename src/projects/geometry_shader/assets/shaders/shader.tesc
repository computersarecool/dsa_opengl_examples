#version 440 core

layout (vertices = 16) out;

const float tess_level = 16.0;

void main()
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = tess_level;
        gl_TessLevelInner[1] = tess_level;
        
        gl_TessLevelOuter[0] = tess_level;
        gl_TessLevelOuter[1] = tess_level;
        gl_TessLevelOuter[2] = tess_level;
        gl_TessLevelOuter[3] = tess_level;
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
