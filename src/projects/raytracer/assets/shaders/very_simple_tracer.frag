#version 440 core

layout (location = 0) out vec3 out_color;

struct sphere
{
    float radius;
    vec3 center;
};

struct ray
{
     vec3 origin;
     vec3 direction;
};

const float epsilon = 0.001;

bool intersect_ray_sphere(in sphere sph, in ray r, out vec3 hit_pos, out vec3 normal)
{
    float t;
    vec3 temp = r.origin - sph.center;
    float b = 2.0 * dot(temp, r.direction);
    float c = dot(temp, temp) - sph.radius * sph.radius;

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
    normal = normalize(hit_pos - sph.center);

    return true;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / 800.0;
    uv = uv * 2.0 - 1.0;

    // Construct a ray
    vec3 ray_origin = vec3(0.0, 0.0, -4.0);
    // TODO: Why is ray direction normalized?
    vec3 ray_direction = normalize(vec3(uv, 1.0));
    ray R = ray (ray_origin, ray_direction);

    // Construct a sphere
    float sphere_radius = 1.0;
    vec3 sphere_origin = vec3(0.0);
    sphere my_sphere = sphere(sphere_radius, sphere_origin);

    // Test if ray hits a sphere
    bool hit;
    vec3 hit_pos;
    vec3 normal;

    hit = intersect_ray_sphere(my_sphere, R, hit_pos, normal);
    if (hit)
    {
        out_color = vec3(1.0);
    }
    else
    {
        out_color = vec3(0.0);
    }
}