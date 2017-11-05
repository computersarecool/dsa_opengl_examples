#version 440 core

out vec4 fragColor;

in TES_OUT
{
    vec3 normal;
} fsIn;

void main()
{
    vec3 normal = normalize(fsIn.normal);

    vec4 c = vec4(1.0, -1.0, 0.0, 0.0) * normal.z + vec4(0.0, 0.0, 0.0, 1.0);

    fragColor = clamp(c, vec4(0.0), vec4(1.0));
}
