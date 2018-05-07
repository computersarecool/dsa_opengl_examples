#version 440 core

layout (vertices = 4) out;

in VS_OUT
{
    vec2 uv;
} tcs_in[];

out TCS_OUT
{
    vec2 uv;
} tcs_out[];

uniform mat4 u_model_view_projection_matrix;

const float scale = 16.0;
const float bias = 1.0;

void main()
{
    if (gl_InvocationID == 0)
    {
        // Get point position in clip space
        vec4 p0 = u_model_view_projection_matrix * gl_in[0].gl_Position;
        vec4 p1 = u_model_view_projection_matrix * gl_in[1].gl_Position;
        vec4 p2 = u_model_view_projection_matrix * gl_in[2].gl_Position;
        vec4 p3 = u_model_view_projection_matrix * gl_in[3].gl_Position;

        // Get point position in NDC
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;
        p3 /= p3.w;

        // Do not tesselate patches behind the viewer
        if (p0.z <= 0.0 ||
            p1.z <= 0.0 ||
            p2.z <= 0.0 ||
            p3.z <= 0.0)
        {
            gl_TessLevelOuter[0] = 0.0;
            gl_TessLevelOuter[1] = 0.0;
            gl_TessLevelOuter[2] = 0.0;
            gl_TessLevelOuter[3] = 0.0;
        }
        else
        {
            // Set outer tesselation to length of quad in NDC * scale + bias
            float length0 = length(p2.xy - p0.xy) * scale + bias;
            float length1 = length(p3.xy - p2.xy) * scale + bias;
            float length2 = length(p3.xy - p1.xy) * scale + bias;
            float length3 = length(p1.xy - p0.xy) * scale + bias;
            gl_TessLevelOuter[0] = length0;
            gl_TessLevelOuter[1] = length1;
            gl_TessLevelOuter[2] = length2;
            gl_TessLevelOuter[3] = length3;

            // Set inner tesselation to the minimum outer level
            gl_TessLevelInner[0] = min(length1, length3);
            gl_TessLevelInner[1] = min(length0, length2);
        }
    }

    // Set output variables
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_out[gl_InvocationID].uv = tcs_in[gl_InvocationID].uv;
}
