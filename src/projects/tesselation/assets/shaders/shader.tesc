#version 440 core

uniform float tessLevel;

layout (vertices = 3) out;

void main()
{
    // gl_InvocationID is the index of the TCS invocation within this patch
    // The initial vertex positions are not changed
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // The conditional is here because if gl_TessLevel is not the same across TCS invocations behavior is undefined
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = tessLevel;
        gl_TessLevelOuter[0] = tessLevel;
        gl_TessLevelOuter[1] = tessLevel;
        gl_TessLevelOuter[2] = tessLevel;
    }
}
