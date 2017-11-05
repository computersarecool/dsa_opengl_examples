#version 440 core

out VS_OUT
{
    vec2 uv;
} vsOut;

const int gridDivisor = 64;
const float halfGridDivisor = 32.0;
const float halfQuadSize = 0.5;

void main()
{
    // Positions for a unit quad in the XZ plane
    const vec4 vertices[] = vec4[](vec4(-halfQuadSize, 0.0, -halfQuadSize, 1.0),
                                   vec4(halfQuadSize, 0.0, -halfQuadSize, 1.0),
                                   vec4(-halfQuadSize, 0.0, halfQuadSize, 1.0),
                                   vec4(halfQuadSize, 0.0, halfQuadSize, 1.0));

    // Calculate XZ offsets for each quad
    float xOffset = float(mod(gl_InstanceID, gridDivisor));
    float zOffset = float(floor(gl_InstanceID / gridDivisor));
    vec2 offsets = vec2(xOffset, zOffset);

    // Assign UV coordinates to each quad
    // This makes each quad's UV coordinates go from offset -> offset + 1 / gridDivisor;
    vsOut.uv = (vertices[gl_VertexID].xz + vec2(halfQuadSize) + offsets) / float(gridDivisor);

    // This evenly distributes the grids around the origin
    gl_Position = vertices[gl_VertexID] + vec4(xOffset - halfGridDivisor, 0.0, zOffset - halfGridDivisor, 0.0);
}
