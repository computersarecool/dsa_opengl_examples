#version 440 core

// TODO:
// Confirm lack of need for point_visible_to_light (in lighting equation)
// Fix reflect and refract outputs
// Add plane intersection test
// Add sphere color

layout (location = 0) out vec3 out_color;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 reflected;
layout (location = 3) out vec3 refracted;
layout (location = 4) out vec3 reflected_color;
layout (location = 5) out vec3 refracted_color;

layout (binding = 0) uniform sampler2D tex_origin;
layout (binding = 1) uniform sampler2D tex_direction;
layout (binding = 2) uniform sampler2D tex_color;

struct ray
{
    vec3 origin;
    vec3 direction;
};

struct sphere
{
    vec3 center;
    float radius;
};

struct light
{
    vec3 center;
};

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

const float epsilon = 0.001;
const float max_t = 1000000.0;

float intersect_ray_sphere(in ray r, in sphere sph, out vec3 hit_pos, out vec3 normal)
{
    vec3 temp = r.origin - sph.center;
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
    normal = normalize(hit_pos - sph.center);

    return t;
}

vec3 light_point(vec3 position, vec3 normal, vec3 V, light l)
{
    vec3 ambient = vec3(0.0);

    vec3 L = normalize(l.center - position);
    vec3 N = normal;
    vec3 R = reflect(-L, N);

    float rim = clamp(dot(N, V), 0.0, 1.0);
    rim = smoothstep(0.0, 1.0, 1.0 - rim);
    float diff = clamp(dot(N, L), 0.0, 1.0);
    float spec = pow(clamp(dot(R, N), 0.0, 1.0), 260.0);

    vec3 rim_color = vec3(0.01, 0.0, 0.03);
    vec3 diff_color = vec3(0.125);
    vec3 spec_color = vec3(0.1);

    return ambient + rim_color * rim + diff_color * diff + spec_color * spec;

}

void main()
{
    // Initialize outputs
    vec3 color = vec3(0.0);
    position = vec3(0.0);
    reflected = vec3(0.0);
    refracted = vec3(0.0);
    reflected_color = vec3(0.0);
    refracted_color = vec3(0.0);

    // Construct a ray
    vec3 ray_origin = texelFetch(tex_origin, ivec2(gl_FragCoord.xy), 0).xyz;
    vec3 ray_direction = normalize(texelFetch(tex_direction, ivec2(gl_FragCoord.xy), 0).xyz);
    ray R = ray(ray_origin, ray_direction);
    vec3 input_color = texelFetch(tex_color, ivec2(gl_FragCoord.xy), 0).rgb;

    // Saved variables
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

    if (min_t < max_t)
    {
        vec3 my_color = vec3(0.0);

        for (int i = 0; i < my_lights.length(); ++i)
        {
            my_color += light_point(hit_position, hit_normal, -R.direction, my_lights[i]);
        }

        //my_color *= S[sphere_index].color.rgb;
        my_color *= input_color;
        vec3 v = normalize(hit_position - R.origin);
        position = hit_position;
        reflected = reflect(v, hit_normal);
        reflected_color = vec3(0.5);///* input_color * */ my_spheres[sphere_index].color.rgb * 0.5;
        refracted = refract(v, hit_normal, 1.73);
        refracted_color = input_color * vec3(0.5);//S[sphere_index].color.rgb * 0.5;

        color = my_color;
    }

    out_color = color;
}
