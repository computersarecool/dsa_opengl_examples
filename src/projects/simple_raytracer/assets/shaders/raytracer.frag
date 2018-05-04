#version 440 core

// TODO:
// Add plane intersection test
// Fix reflect and refract outputs

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

// Object intersection constants
const int object_type_miss = -1;
const int object_type_sphere = 0;
const int object_type_plane = 1;

// Structs
// An intersection holds several values:
// - `t` a scalar which denotes a point along the incident ray
// - `object_type`
// - `object_index`
// - `incident` the direction vector of the incident ray
// - `position` the position where the hit happened
// - `normal` vector of the object that was hit calculated at `position`
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

//layout (std140, binding = 2) uniform PLANES
 //{
//    plane my_planes[6];
//};

layout (std140, binding = 3) uniform LIGHTS
{
    light my_lights[2];
};

plane my_planes[6] = {
    {vec4(1, 0, 0, 0), vec4(-20, 0, 0, 0)},
    {vec4(-1, 0, 0, 0), vec4(20, 0, 0, 0)},
    {vec4(0, 1, 0, 0), vec4(0, -20, 0, 0)},
    {vec4(0, -1, 0, 0), vec4(0, 20, 0, 0)},
    {vec4(0, 0, 1, 0), vec4(0, 0, -20, 0)},
    {vec4(0, 0, -1, 0), vec4(0, 0, 20, 0)}
};


// Intersection test constants
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

float intersect_ray_plane(in ray r, in plane pln)
{
    // We can write the equation for a plane, given its normal vector and
    // a single point `p` that lies on the plane. We can test if a different
    // point `v` is on the plane via the following formula:
    //
    //      dot(v - p, n) = 0
    //
    // Now, we can substitute in the equation for our ray:
    //
    //      dot((o - t * d) - p, n) = 0
    //
    // Solving for `t` in the equation above, we obtain:
    //
    //      numerator = dot(p - o, n)
    //      denominator = dot(d, n)
    //      t = numerator / denominator
    //
    // If the ray is perfectly parallel to the plane, then the denominator
    // will be zero. If the ray intersects the plane from behind, then `t`
    // will be negative. We want to avoid both of these cases.
    float denominator = dot(r.direction, pln.normal.xyz);
    float numerator = dot(pln.center.xyz - r.origin, pln.normal.xyz);

    // The ray is (basically) parallel to the plane
    if (denominator > epsilon)
    {
        return 0.0;
    }

    float t = numerator / denominator;

    if (t > 0.0)
    {
        return t;
    }

    // The ray intersected the plane from behind
    return 0.0;
}

vec3 light_point(vec3 position, vec3 normal, vec3 V, light my_light)
{
    const vec3 ambient = vec3(0.0);

    vec3 L = normalize(my_light.center.xyz - position);
    vec3 N = normal;
    vec3 R = reflect(-L, N);

    float rim = clamp(dot(N, V), 0.0, 1.0);
    rim = smoothstep(0.0, 1.0, 1.0 - rim);
    float diff = clamp(dot(N, L), 0.0, 1.0);
    float spec = pow(clamp(dot(R, N), 0.0, 1.0), 260.0);

    const vec3 rim_color = vec3(0.01, 0.0, 0.03);
    const vec3 diff_color = vec3(0.45);
    const vec3 spec_color = vec3(0.1);

    return ambient + rim_color * rim + diff_color * diff + spec_color * spec;
}

void trace_ray(out vec3 out_color, out vec3 position, out vec3 reflected, out vec3 refracted, out vec3 reflected_color, out vec3 refracted_color)
{

    // Construct a ray
    vec3 ray_origin = texelFetch(origin_texture, ivec2(gl_FragCoord.xy), 0).xyz;
    vec3 ray_direction = normalize(texelFetch(direction_texture, ivec2(gl_FragCoord.xy), 0).xyz);
    ray R = ray(ray_origin, ray_direction);

    // Intersection test struct
    intersection intersect_data;
    intersect_data.t = max_t;

    // Temporary intersection test variables
    vec3 temp_hit_position;
    vec3 temp_hit_normal;

    // Test if ray hits a sphere
    for (int i = 0; i < my_spheres.length(); ++i)
    {
        float temp_t = intersect_ray_sphere(R, my_spheres[i], temp_hit_position, temp_hit_normal);
        if (temp_t != 0.0)
        {
            if (temp_t < intersect_data.t)
            {
                intersect_data.t = temp_t;
                intersect_data.object_type = object_type_sphere;
                intersect_data.object_index = i;
                intersect_data.position = temp_hit_position;
                intersect_data.normal = temp_hit_normal;
            }
        }
    }

    // Intersect ray plane
    for(int i = 0; i < my_planes.length(); ++i)
    {
        float temp_t = intersect_ray_plane(R, my_planes[i]);
        if (temp_t != 0.0)
        {
            // Was the intersection closer than any previous one?
            if (temp_t < intersect_data.t)
            {
                intersect_data.t = temp_t;
                intersect_data.object_type = object_type_plane;
                intersect_data.object_index = i;
                intersect_data.position = R.origin + R.direction * temp_t;
                intersect_data.normal = normalize(my_planes[i].normal.xyz);
            }
        }
    }

    // Color point if there is a hit inside bounds
    if (intersect_data.t < max_t)
    {
        for (int i = 0; i < my_lights.length(); ++i)
        {
            // TODO: Check if visibile to light here (for shadows)
            out_color += light_point(intersect_data.position, intersect_data.normal, -R.direction, my_lights[i]);
        }

        vec3 input_color = texelFetch(color_texture, ivec2(gl_FragCoord.xy), 0).rgb;
        out_color *= input_color;

        if (intersect_data.object_type == object_type_sphere)
        {
            out_color *= my_spheres[intersect_data.object_index].color.rgb;
        }

        else if (intersect_data.object_type == object_type_plane)
        {
            out_color = vec3(0.2);
           // out_color *= my_planes[intersect_data.object_index].color.rgb;
        }

        // Can this be simplified to just ray direction
        vec3 v = normalize(intersect_data.position - R.origin);
        position = intersect_data.position;
        reflected = reflect(v, intersect_data.normal);
        // Multiply by .5 to make slighly less bright
        reflected_color = my_spheres[intersect_data.object_index].color.rgb * 0.5;

        // Look up refract function
        refracted = refract(v, intersect_data.normal, 1.73);
        refracted_color = input_color * vec3(0.5);//S[sphere_index].color.rgb * 0.5;
    }
}

void main()
{
    trace_ray(out_color, position, reflected, refracted, reflected_color, refracted_color);
}
