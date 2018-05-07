#version 440 core

const float epsilon = 0.001;

uniform mat4 u_model_view_matrix;
uniform mat4 u_projection_matrix;

layout (quads, equal_spacing, cw) in;

out TES_OUT
{
    vec3 normal;
} tes_out;

vec4 quadratic_bezier(vec4 A, vec4 B, vec4 C, float t)
{
    vec4 D = mix(A, B, t);
    vec4 E = mix(B, C, t);

    return mix(D, E, t);
}

vec4 cubic_bezier(vec4 A, vec4 B, vec4 C, vec4 D, float t)
{
    vec4 E = mix(A, B, t);
    vec4 F = mix(B, C, t);
    vec4 G = mix(C, D, t);

    return quadratic_bezier(E, F, G, t);
}

vec4 evaluate_patch(vec2 at)
{
    vec4 point_coords[4];

    for (int i = 0; i < point_coords.length; i++)
    {
        point_coords[i] = cubic_bezier(
            gl_in[i + 0].gl_Position,
            gl_in[i + 4].gl_Position,
            gl_in[i + 8].gl_Position,
            gl_in[i + 12].gl_Position,
            at.y
        );
    }

    return cubic_bezier(point_coords[0], point_coords[1], point_coords[2], point_coords[3], at.x);
}

void main()
{
    vec4 position = evaluate_patch(gl_TessCoord.xy);
    vec4 position_y_offset = evaluate_patch(gl_TessCoord.xy + vec2(0.0, epsilon));
    vec4 position_x_offset = evaluate_patch(gl_TessCoord.xy + vec2(epsilon, 0.0));

    vec3 v1 = normalize(position_y_offset.xyz - position.xyz);
    vec3 v2 = normalize(position_x_offset.xyz - position.xyz);

    tes_out.normal = cross(v1, v2);

    gl_Position = position;
}
