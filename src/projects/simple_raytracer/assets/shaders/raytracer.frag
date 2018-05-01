#version 440 core

layout (location = 0) out vec3 out_color;

layout (binding = 0) uniform sampler2D tex_origin;
layout (binding = 1) uniform sampler2D tex_direction;

struct sphere
{
    vec3 center;
    float radius;
};

struct ray
{
    vec3 origin;
    vec3 direction;
};

layout (std140, binding = 0) uniform SPHERES
{
    sphere my_spheres[4];
};

// TODO: What is epsilon?
const float epsilon = 0.001;

bool intersect_ray_sphere(in sphere sph, in ray r, out vec3 hit_pos, out vec3 normal)
{
    vec3 sphere_center = sph.center;
    float sphere_radius = sph.radius;

    float t;
    vec3 temp = r.origin - sphere_center;
    float b = 2.0 * dot(temp, r.direction);
    float c = dot(temp, temp) - sphere_radius * sphere_radius;

    float discriminant = b * b - 4.0 * c;

    // Avoid taking the square root of a negative number.
    if (discriminant < 0.0)
    {
        return false;
    }

    discriminant = sqrt(discriminant);
    float t0 = -b + discriminant;
    float t1 = -b - discriminant;

    // We want to take the smallest positive root.
    if (t1 > epsilon)
    {
        t = t1 * 0.5;
    }
    else if (t0 > epsilon)
    {
        t = t0 * 0.5;
    }
    else
    {
        return false;
    }

    hit_pos = r.origin + t * r.direction;
    normal = normalize(hit_pos - sphere_center);

    return true;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / 800.0;
    uv = uv * 2.0 - 1.0;
//    out_color = normalize(vec3(uv, 1.0));

    out_color = normalize(texelFetch(tex_direction, ivec2(gl_FragCoord.xy), 0).xyz);

//    // Construct a ray
//    vec3 ray_origin = vec3(0.0, 0.0, -4.0);
//    vec3 ray_direction = normalize(vec3(uv, 1.0));
//    ray R = ray (ray_origin, ray_direction);
//
//    // Test if the ray hits a sphere
//    vec3 hit_pos = {100000.0, 100000.0, 100000.0};
//    bool hit_something = false;
//    vec3 normal;
//    for (int i = 0; i < my_spheres.length(); ++i)
//    {
//        vec3 temp_hit;
//        if (intersect_ray_sphere(my_spheres[i], R, temp_hit, normal))
//        {
//            // There is a hit but is it closer than any previous?
//            if (length(ray_origin - temp_hit) < length(ray_origin - hit_pos))
//            {
//                hit_pos = temp_hit;
//                out_color = vec3(1.0);
//                hit_something = true;
//            }
//        }
//    }
//    if (!hit_something)
//    {
//        out_color = vec3(0.0);
//    }
}