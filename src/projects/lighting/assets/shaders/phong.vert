#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;

out Vertex {
    vec3 worldSpacePos;
    vec3 worldSpaceNorm;
    vec2 uv;
} vVert;

void main()
{
    vec4 worldSpacePos = uModelMatrix * vec4(aPos, 1.0);
    gl_Position = uProjectionMatrix * uViewMatrix * worldSpacePos;

    vVert.worldSpacePos = worldSpacePos.xyz;
    vVert.worldSpaceNorm = normalize(uNormalMatrix * aNormal);
    vVert.uv = aUv;
}
