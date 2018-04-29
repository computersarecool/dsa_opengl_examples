#version 440 core

// Outputs
layout (location = 0) out vec3 color;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 reflected;
layout (location = 3) out vec3 refracted;
layout (location = 4) out vec3 reflected_color;
layout (location = 5) out vec3 refracted_color;

// Sampler inputs
layout (binding = 0) uniform sampler2D tex_origin;
layout (binding = 1) uniform sampler2D tex_direction;
layout (binding = 2) uniform sampler2D tex_color;

uniform int num_spheres = 7;
const float epsilon = .001;

struct ray
{
    vec3 origin;
    vec3 direction;
};

struct sphere
{
    vec3 center;
    float radius;
    vec4 color;
};

layout (std140, binding = 1) uniform SPHERES
{
    sphere spheres[128];
};

//bool intersect_ray_sphere(ray R, sphere S, out vec3 hitpos, out vec3 normal)
//{
//    vec3 v = R.origin - S.center;
//    float a = 1.0;
//    float b = 2.0 * dot(R.direction, v);
//    float c = dot(v, v) - pow(S.radius, 2);
//
//    float num = pow(b, 2) - 4.0 * a * c;
//
//    if (num < 0.0)
//    {
//        return false;
//    }
//
//    float d = sqrt(num);
//    float e = 1.0 / (2.0 * a);
//
//    float t1 = (-b - d) * e;
//    float t2 = (-b + d) * e;
//    float t = min(max(t1, 0.0), max(t2, 0.0));
//
//    if (t == 0.0)
//    {
//        return false;
//    }
//
//    hitpos = R.origin + t * R.direction;
//    normal = normalize(hitpos - S.center);
//
//    return true;
//}


bool intersect_ray_sphere(in sphere sph, in ray r, out vec3 hitpos, out vec3 normal)
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

    hitpos = r.origin + t *r.direction;
    normal = normalize(hitpos - sph.center);

    return true;
}




void main()
{
    // Initialize outputs
    color = vec3(0.0);
    position = vec3(0.0);
    reflected = vec3(0.0);
    refracted = vec3(0.0);
    reflected_color = vec3(0.0);
    refracted_color = vec3(0.0);

    // Construct a ray
    ray R;
    R.origin = texelFetch(tex_origin, ivec2(gl_FragCoord.xy), 0).xyz;
    R.direction = normalize(texelFetch(tex_direction, ivec2(gl_FragCoord.xy), 0).xyz);
    R.direction.z *= -1.0;
    R.origin += R.direction * 0.01;

    // Test if ray hits a sphere
    bool hit;
    vec3 hitpos;
    vec3 normal;
    for (int i = 0; i < num_spheres; i++)
    {
        hit = intersect_ray_sphere(spheres[i], R, hitpos, normal);
        if (hit)
        {
            color = vec3(1.0);
        }
        else
        {
            color = vec3(1.0, 0.0, 0.0);
        }
    }
}
