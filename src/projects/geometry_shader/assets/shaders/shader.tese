#version 440 core

const float epsilon = 0.001;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

layout (quads, equal_spacing, cw) in;

out TES_OUT
{
    vec3 normal;
} tesOut;

vec4 quadraticBezier(vec4 A, vec4 B, vec4 C, float t)
{
    vec4 D = mix(A, B, t);
    vec4 E = mix(B, C, t);

    return mix(D, E, t);
}

vec4 cubicBezier(vec4 A, vec4 B, vec4 C, vec4 D, float t)
{
    vec4 E = mix(A, B, t);
    vec4 F = mix(B, C, t);
    vec4 G = mix(C, D, t);

    return quadraticBezier(E, F, G, t);
}

vec4 evaluatePatch(vec2 at)
{
    vec4 pointCoords[4];

    for (int i = 0; i < pointCoords.length; i++)
    {
        pointCoords[i] = cubicBezier(gl_in[i + 0].gl_Position,
                                     gl_in[i + 4].gl_Position,
                                     gl_in[i + 8].gl_Position,
                                     gl_in[i + 12].gl_Position,
                                     at.y);
    }

    return cubicBezier(pointCoords[0], pointCoords[1], pointCoords[2], pointCoords[3], at.x);
}



void main()
{
    vec4 position = evaluatePatch(gl_TessCoord.xy);
    vec4 position_y_offset = evaluatePatch(gl_TessCoord.xy + vec2(0.0, epsilon));
    vec4 position_x_offset = evaluatePatch(gl_TessCoord.xy + vec2(epsilon, 0.0));

    vec3 v1 = normalize(position_y_offset.xyz - position.xyz);
    vec3 v2 = normalize(position_x_offset.xyz - position.xyz);

    tesOut.normal = cross(v1, v2);

    gl_Position = position;
}
