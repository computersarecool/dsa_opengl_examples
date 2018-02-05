#version 440 core

in GS_OUT
{
    flat vec3 color[4];
    vec2 uv;
} fsIn;

out vec4 fragColor;

void main()
{
    vec3 c1 = mix(fsIn.color[0], fsIn.color[1], fsIn.uv.x);
    vec3 c2 = mix(fsIn.color[2], fsIn.color[3], fsIn.uv.x);
    fragColor = vec4(mix(c1, c2, fsIn.uv.y), 1.0);
}
