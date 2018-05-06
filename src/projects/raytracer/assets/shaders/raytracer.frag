#version 440 core

// Outputs
layout (location = 0) out vec3 out_composite_color;
layout (location = 1) out vec3 out_origin;
layout (location = 2) out vec3 out_direction;
layout (location = 3) out vec3 out_reflection_color;

// Samplers
layout (binding = 0) uniform sampler2D ray_origin_texture;
layout (binding = 1) uniform sampler2D ray_direction_texture;
layout (binding = 2) uniform sampler2D ray_reflection_color_texture;

// Object intersection type constants
const int object_type_miss = -1;
const int object_type_sphere = 0;
const int object_type_plane = 1;

// Structs
struct intersection
{
    float t;
    int object_type;
    int object_index;
    vec3 incident;
    vec3 position;
    vec3 normal;
};

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

struct plane
{
    vec4 normal;
    vec4 center;
    vec4 color;
};

struct light
{
    vec4 center;
};

// UBOs
layout (std140, binding = 0) uniform SPHERES
{
    sphere my_spheres[4];
};

layout (std140, binding = 1) uniform PLANES
 {
    plane my_planes[6];
};

layout (std140, binding = 2) uniform LIGHTS
{
    light my_lights[2];
};

// Intersection test constants
const float epsilon = 0.001;
const float max_t = 1000000.0;

bool intersect_ray_sphere(in ray my_ray, in sphere sph, out vec3 hit_pos, out vec3 normal, out float t)
{
    vec3 temp = my_ray.origin - sph.center.xyz;
    float b = 2.0 * dot(temp, my_ray.direction);
    float c = dot(temp, temp) - sph.radius * sph.radius;

    float discriminant = b * b - 4.0 * c;

    // Avoid taking the square root of a negative number
    if (discriminant < 0.0)
    {
        return false;
    }

    discriminant = sqrt(discriminant);
    float t0 = -b + discriminant;
    float t1 = -b - discriminant;

    t = 0.0;

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
        return false;
    }

    hit_pos = my_ray.origin + t * my_ray.direction;
    normal = normalize(hit_pos - sph.center.xyz);

    return true;
}

bool intersect_ray_plane(in ray my_ray, in plane pln, out float t)
{
    float denominator = dot(my_ray.direction, pln.normal.xyz);
    float numerator = dot(pln.center.xyz - my_ray.origin, pln.normal.xyz);

    // The ray is (basically) parallel to the plane
    if (denominator > epsilon)
    {
        return false;
    }

    t = numerator / denominator;

    if (t > 0.0)
    {
        return true;
    }

    // The ray intersected the plane from behind
    return false;
}

bool point_visible_to_light(vec3 point, vec3 L)
{
    vec3 shadow_ray_direction = normalize(L - point);
    vec3 shadow_ray_origin = point + shadow_ray_direction * 0.001;
    ray shadow_ray = ray(shadow_ray_origin, shadow_ray_direction);

    // Temporary intersection test variables
    vec3 temp_hit_position;
    vec3 temp_hit_normal;
    float temp_t;

    // Test if shadow ray hits a sphere
    for (int index = 0; index < my_spheres.length(); ++index)
    {
        if (intersect_ray_sphere(shadow_ray, my_spheres[index], temp_hit_position, temp_hit_normal, temp_t))
        {
            return false;
        }
    }

    return true;
}

vec3 light_point(vec3 position, vec3 normal, vec3 V, light my_light)
{
    // Lighting constants
    const vec3 ambient = vec3(0.0);
    const vec3 diffuse_color = vec3(0.45);
    const vec3 rim_color = vec3(0.01);
    const vec3 specular_color = vec3(0.1);
    const float specular_term = 260.0;

    if (!point_visible_to_light(position, my_light.center.xyz))
    {
        return ambient;
    }

    vec3 L = normalize(my_light.center.xyz - position);
    vec3 my_ray = reflect(-L, normal);

    // Rim light contribution
    float rim_amount = clamp(dot(normal, V), 0.0, 1.0);
    rim_amount = smoothstep(0.0, 1.0, 1.0 - rim_amount);

    // Diffuse contribution
    float diffuse_amount = clamp(dot(normal, L), 0.0, 1.0);

    // Specular contribution
    float specular_amount = pow(clamp(dot(my_ray, normal), 0.0, 1.0), specular_term);

    return ambient + rim_color * rim_amount + diffuse_color * diffuse_amount + specular_color * specular_amount;
}

void trace_ray(out vec3 out_composite_color, out vec3 out_origin, out vec3 out_direction, out vec3 out_reflection_color)
{
    // Construct a ray
    vec3 ray_origin = texelFetch(ray_origin_texture, ivec2(gl_FragCoord.xy), 0).xyz;
    vec3 ray_direction = normalize(texelFetch(ray_direction_texture, ivec2(gl_FragCoord.xy), 0).xyz);
    ray my_ray = ray(ray_origin, ray_direction);

    // Intersection test data
    intersection intersect_data;
    intersect_data.t = max_t;

    // Temporary intersection test variables
    vec3 temp_hit_position;
    vec3 temp_hit_normal;

    // Test if ray hits a sphere
    for (int index = 0; index < my_spheres.length(); ++index)
    {
        float temp_t;
        if (intersect_ray_sphere(my_ray, my_spheres[index], temp_hit_position, temp_hit_normal, temp_t))
        {
            // Was the intersection closer than any previous one?
            if (temp_t < intersect_data.t)
            {
                intersect_data.t = temp_t;
                intersect_data.object_type = object_type_sphere;
                intersect_data.object_index = index;
                intersect_data.position = temp_hit_position;
                intersect_data.normal = temp_hit_normal;
            }
        }
    }

    // Test if ray hits a plane
    for(int index = 0; index < my_planes.length(); ++index)
    {
        float temp_t;
        if (intersect_ray_plane(my_ray, my_planes[index], temp_t))
        {
            if (temp_t < intersect_data.t)
            {
                intersect_data.t = temp_t;
                intersect_data.object_type = object_type_plane;
                intersect_data.object_index = index;
                intersect_data.position = my_ray.origin + my_ray.direction * temp_t;
                intersect_data.normal = normalize(my_planes[index].normal.xyz);
            }
        }
    }

    // Color point if there is a hit inside the bounds and calculate reflection
    if (intersect_data.t < max_t)
    {
        out_origin = intersect_data.position;
        out_direction = reflect(my_ray.direction, intersect_data.normal);

        // Calculate lighting at this point
        for (int index = 0; index < my_lights.length(); ++index)
        {
            out_composite_color += light_point(intersect_data.position, intersect_data.normal, -my_ray.direction, my_lights[index]);
        }

        // Multiply the composite color by the accumulated relection color (which is initially set to 1)
        vec3 accumulated_reflection_color = texelFetch(ray_reflection_color_texture, ivec2(gl_FragCoord.xy), 0).rgb;
        out_composite_color *= accumulated_reflection_color;

        if (intersect_data.object_type == object_type_sphere)
        {
            out_composite_color *= my_spheres[intersect_data.object_index].color.rgb;
            out_reflection_color = my_spheres[intersect_data.object_index].color.rgb * 0.5;
        }

        else if (intersect_data.object_type == object_type_plane)
        {
           out_composite_color *= my_planes[intersect_data.object_index].color.rgb;
           out_reflection_color = my_planes[intersect_data.object_index].color.rgb * 0.5;
        }
    }
}

void main()
{
    trace_ray(out_composite_color, out_origin, out_direction, out_reflection_color);
}
