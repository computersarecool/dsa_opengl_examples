#version 440 core

layout (quads, fractional_odd_spacing) in;

in TCS_OUT
{
    vec2 uv;
} tesIn[];

out TES_OUT
{
    vec2 uv;
} tesOut;

uniform sampler2D uTexDisplacement;
uniform mat4 uModelViewProjectionMatrix;
uniform float uDisplacementWeight;

void main()
{
    // Calculate UV coordinate by interpolating the passed UV in X and then mix betweeen bottom and top
    vec2 uvBottom = mix(tesIn[0].uv, tesIn[1].uv, gl_TessCoord.x);
    vec2 uvTop = mix(tesIn[2].uv, tesIn[3].uv, gl_TessCoord.x);
    vec2 uv = mix(uvTop, uvBottom, gl_TessCoord.y);

    // Interpolate the position in the same way
    vec4 pBottom = mix(gl_in[0].gl_Position,
                        gl_in[1].gl_Position,
                        gl_TessCoord.x);

    vec4 pTop = mix(gl_in[2].gl_Position,
                     gl_in[3].gl_Position,
                     gl_TessCoord.x);

    vec4 p = mix(pTop, pBottom, gl_TessCoord.y);

    // Displacement map
    // Offset the point's Y position based on texture value at its UV coordinate
    p.y += texture(uTexDisplacement, uv).r * uDisplacementWeight;

    gl_Position = uModelViewProjectionMatrix * p;
    tesOut.uv = uv;
}
