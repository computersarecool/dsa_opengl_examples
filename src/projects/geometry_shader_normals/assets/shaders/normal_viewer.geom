#version 440 core

layout (triangles) in;
layout (line_strip, max_vertices = 8) out;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform float uNormalLength;

in VS_OUT
{
    vec3 normal;
} gsIn[];


void main()
{
    vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 faceNormal = normalize(cross(ab, ac));

    vec4 triCentroid = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;


    gl_Position = uProjectionMatrix * uModelViewMatrix * triCentroid;
    EmitVertex();

    gl_Position = uProjectionMatrix * uModelViewMatrix * (triCentroid + vec4(faceNormal * uNormalLength, 0.0));
    EmitVertex();
    EndPrimitive();


    // Emit a line strip at each normal
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = uProjectionMatrix * uModelViewMatrix * gl_in[i].gl_Position;
        EmitVertex();

        gl_Position = uProjectionMatrix * uModelViewMatrix * (gl_in[i].gl_Position + vec4(gsIn[i].normal * uNormalLength, 0.0));
        EmitVertex();
        EndPrimitive();
    }
}
