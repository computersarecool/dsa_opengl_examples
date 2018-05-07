#version 440 core

out VS_OUT
{
    vec2 uv;
} vsOut;

const int grid_divisor = 64;
const float half_grid_divisor = 32.0;
const float half_quad_size = 0.5;

void main()
{
    // Positions for a unit quad in the XZ plane
    const vec4 vertices[] = {
        {-half_quad_size, 0.0, -half_quad_size, 1.0},
        {half_quad_size, 0.0, -half_quad_size, 1.0},
        {-half_quad_size, 0.0, half_quad_size, 1.0},
        {half_quad_size, 0.0, half_quad_size, 1.0}
    };

    // Calculate XZ offsets for each quad
    float x_offset = float(mod(gl_InstanceID, grid_divisor));
    float z_offset = float(floor(gl_InstanceID / grid_divisor));
    vec2 offsets = vec2(x_offset, z_offset);

    // Assign UV coordinates to each quad
    // This makes each quad's UV coordinates go from offset -> offset + 1 / grid_divisor;
    vsOut.uv = (vertices[gl_VertexID].xz + vec2(half_quad_size) + offsets) / float(grid_divisor);

    // This evenly distributes the grids around the origin
    gl_Position = vertices[gl_VertexID] + vec4(x_offset - half_grid_divisor, 0.0, z_offset - half_grid_divisor, 0.0);
}
