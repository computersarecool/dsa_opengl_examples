#version 440 core

layout (location = 0) out vec3 out_color;

struct sphere
{
    vec4 center_and_radius;
    // vec3 center;
    // float radius
};

struct ray
{
    vec3 origin;
    vec3 direction;
};

layout (std140, binding = 0) uniform SPHERES
{
    vec4 my_spheres[4];
};

// TODO: What is epsilon?
const float epsilon = 0.001;

bool intersect_ray_sphere(in vec4 sph, in ray r, out vec3 hit_pos, out vec3 normal)
{
    vec3 sphere_center = sph.xyz;
    float sphere_radius = sph.w;

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

    // Construct a ray
    vec3 ray_origin = vec3(0.0, 0.0, -4.0);
    vec3 ray_direction = normalize(vec3(uv, 1.0));
    ray R = ray (ray_origin, ray_direction);

    // Test if the ray hits a sphere
    vec3 hit_pos;
    vec3 normal;
    bool hit;
    for (int i = 0; i < my_spheres.length; ++i)
    {
        hit = intersect_ray_sphere(my_spheres[i], R, hit_pos, normal);
        if (hit)
        {
            out_color = vec3(1.0);
        }
        else
        {
            out_color = vec3(0.0);
        }
    }
}