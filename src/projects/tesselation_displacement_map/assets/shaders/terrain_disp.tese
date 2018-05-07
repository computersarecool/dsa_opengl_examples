#version 440 core

layout (quads, fractional_odd_spacing) in;

in TCS_OUT
{
    vec2 uv;
} tes_in[];

out TES_OUT
{
    vec2 uv;
} tes_out;

uniform sampler2D u_tex_displacement;
uniform mat4 u_model_view_projection_matrix;
uniform float u_displacement_weight;

void main()
{
    // Calculate UV coordinate by interpolating the passed UV in X and then mix betweeen bottom and top
    vec2 uv_bottom = mix(tes_in[0].uv, tes_in[1].uv, gl_TessCoord.x);
    vec2 uv_top = mix(tes_in[2].uv, tes_in[3].uv, gl_TessCoord.x);
    vec2 uv = mix(uv_top, uv_bottom, gl_TessCoord.y);

    // Interpolate the position in the same way
    vec4 p_bottom = mix(gl_in[0].gl_Position,
                        gl_in[1].gl_Position,
                        gl_TessCoord.x);

    vec4 p_top = mix(gl_in[2].gl_Position,
                     gl_in[3].gl_Position,
                     gl_TessCoord.x);

    vec4 p = mix(p_top, p_bottom, gl_TessCoord.y);

    // Displacement map
    // Offset the point's Y position based on texture value at its UV coordinate
    p.y += texture(u_tex_displacement, uv).r * u_displacement_weight;

    gl_Position = u_model_view_projection_matrix * p;
    tes_out.uv = uv;
}
