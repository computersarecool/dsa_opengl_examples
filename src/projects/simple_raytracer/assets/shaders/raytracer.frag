#version 440 core

// TODO:
// Fix reflect and refract outputs
// Add plane intersection test

// Outputs
layout (location = 0) out vec3 out_color;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 reflected;
layout (location = 3) out vec3 refracted;
layout (location = 4) out vec3 reflected_color;
layout (location = 5) out vec3 refracted_color;

// Samplers
layout (binding = 0) uniform sampler2D origin_texture;
layout (binding = 1) uniform sampler2D direction_texture;
layout (binding = 2) uniform sampler2D color_texture;

// Scene structs
struct ray
{
    vec3 origin;
    vec3 direction;
};

struct sphere
{
    vec4 center;
    vec4 color;
    float radius;
};

struct light
{
    vec4 center;
};

// UBOs
layout (std140, binding = 1) uniform SPHERES
{
    sphere my_spheres[4];
};

layout (std140, binding = 2) uniform PLANES
{
    vec4 my_planes[3];
};

layout (std140, binding = 3) uniform LIGHTS
{
    light my_lights[2];
};

// Scene constants
const float epsilon = 0.001;
const float max_t = 1000000.0;

float intersect_ray_sphere(in ray r, in sphere sph, out vec3 hit_pos, out vec3 normal)
{
    vec3 temp = r.origin - sph.center.xyz;
    float b = 2.0 * dot(temp, r.direction);
    float c = dot(temp, temp) - sph.radius * sph.radius;

    float discriminant = b * b - 4.0 * c;

    // Avoid taking the square root of a negative number
    if (discriminant < 0.0)
    {
        return 0.0;
    }

    discriminant = sqrt(discriminant);
    float t0 = -b + discriminant;
    float t1 = -b - discriminant;

    float t = 0.0;

    // We want to take the smallest positive root
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
        return 0.0;
    }

    hit_pos = r.origin + t * r.direction;
    normal = normalize(hit_pos - sph.center.xyz);

    return t;
}

vec3 light_point(vec3 position, vec3 normal, vec3 V, light my_light)
{
    vec3 ambient = vec3(0.0);

    vec3 L = normalize(my_light.center.xyz - position);
    vec3 N = normal;
    vec3 R = reflect(-L, N);

    float rim = clamp(dot(N, V), 0.0, 1.0);
    rim = smoothstep(0.0, 1.0, 1.0 - rim);
    float diff = clamp(dot(N, L), 0.0, 1.0);
    float spec = pow(clamp(dot(R, N), 0.0, 1.0), 260.0);

    vec3 rim_color = vec3(0.01, 0.0, 0.03);
    vec3 diff_color = vec3(0.45);
    vec3 spec_color = vec3(0.1);

    return ambient + rim_color * rim + diff_color * diff + spec_color * spec;
}

void main()
{
    // Initialize outputs
    out_color = vec3(0.0);
    position = vec3(0.0);
    reflected = vec3(0.0);
    refracted = vec3(0.0);
    reflected_color = vec3(0.0);
    refracted_color = vec3(0.0);

    // Construct a ray
    vec3 ray_origin = texelFetch(origin_texture, ivec2(gl_FragCoord.xy), 0).xyz;
    vec3 ray_direction = normalize(texelFetch(direction_texture, ivec2(gl_FragCoord.xy), 0).xyz);
    ray R = ray(ray_origin, ray_direction);

    // Saved intersection test variables
    vec3 hit_position = vec3(0.0);
    vec3 hit_normal = vec3(0.0);
    float min_t = max_t;
    uint sphere_index = 0;

    // Temporary intersection test variables
    vec3 temp_hit_position;
    vec3 temp_hit_normal;

    // Test if ray hits a sphere
    for (int i = 0; i < my_spheres.length(); ++i)
    {
        float t = intersect_ray_sphere(R, my_spheres[i], temp_hit_position, temp_hit_normal);
        if (t != 0.0)
        {
            if (t < min_t)
            {
                min_t = t;
                hit_position = temp_hit_position;
                hit_normal = temp_hit_normal;
                sphere_index = i;
            }
        }
    }

    // Color point if there is a hit inside bounds
    if (min_t < max_t)
    {
        for (int i = 0; i < my_lights.length(); ++i)
        {
            // Insert check if visibile to light here (for shadows)
            out_color += light_point(hit_position, hit_normal, -R.direction, my_lights[i]);
        }

        vec3 input_color = texelFetch(color_texture, ivec2(gl_FragCoord.xy), 0).rgb;
        out_color *= input_color;
        out_color *= my_spheres[sphere_index].color.rgb;

        // Probably could just be ray direction
        vec3 v = normalize(hit_position - R.origin);
        position = hit_position;
        reflected = reflect(v, hit_normal);
        // Multiply by .5 to make slighly less bright
        reflected_color = my_spheres[sphere_index].color.rgb * 0.5;

        // Look up refract function
        refracted = refract(v, hit_normal, 1.73);
        refracted_color = input_color * vec3(0.5);//S[sphere_index].color.rgb * 0.5;
    }
}
