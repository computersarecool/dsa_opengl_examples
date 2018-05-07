#version 440 core

layout (triangles) in;
layout (line_strip, max_vertices = 8) out;

uniform mat4 u_model_view_matrix;
uniform mat4 u_projection_matrix;
uniform float u_normal_length;

in VS_OUT
{
    vec3 normal;
} gsIn[];


void main()
{
    vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 face_normal = normalize(cross(ab, ac));

    vec4 tri_centroid = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;


    gl_Position = u_projection_matrix * u_model_view_matrix * tri_centroid;
    EmitVertex();

    gl_Position = u_projection_matrix * u_model_view_matrix * (tri_centroid + vec4(face_normal * u_normal_length, 0.0));
    EmitVertex();
    EndPrimitive();


    // Emit a line strip at each normal
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = u_projection_matrix * u_model_view_matrix * gl_in[i].gl_Position;
        EmitVertex();

        gl_Position = u_projection_matrix * u_model_view_matrix * (gl_in[i].gl_Position + vec4(gsIn[i].normal * u_normal_length, 0.0));
        EmitVertex();
        EndPrimitive();
    }
}
