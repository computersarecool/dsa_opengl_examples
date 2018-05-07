#version 440 core

// Comments show where you could use the following features:
// shadows, ambient lights, emit and constant colors, light multiplying alpha, fog, "darkness emit"
// and the following maps:
// projection, environment, alpha, color
uniform vec4 u_vertex_color;
uniform vec3 u_camera_position;

// Light properties
uniform vec3 u_light_color;
uniform vec3 u_light_position;

// Spotlight properties
uniform vec3 u_light_direction;
uniform float u_light_cutoff;
uniform float u_light_outer_cutoff;

// Material uniforms
uniform vec4 u_diffuse_color;
uniform vec4 u_ambient_color;
uniform vec3 u_specular_color;
uniform float u_shininess;
uniform float u_shadow_strength;
uniform vec3 u_shadow_color;
uniform sampler2D u_diffuse_texture;
uniform sampler2D u_specular_texture;

in VS_OUT {
    vec3 world_space_position;
    vec3 world_space_normal;
    vec2 uv;
} v_vert;

layout(location = 0) out vec4 frag_color;

const int num_lights = 1;
const float linear_light_attenuation = 0.09;
const float cubic_light_attenuation = 0.032;

void calculatePhong(inout vec3 diffuse_contrib, inout vec3 specular_contrib, int light_index, vec3 world_space_position,
                    vec3 normal, float shadow_strength, vec3 shadow_color, vec3 view_vector, float shininess)
{
    // Note: Use light_index to index into an array of lights any time u_light_color is used

    // If using a projection map:
    //   true_light_color = u_light_color * projectionMapColor
    // else:

    vec3 true_light_color = u_light_color;

    // If using shadows:
    //   if (in a shadow)
    //      light_color_sum = mix(true_light_color, shadow_color, shadow_strength)
    //    else:
    //      light_color_sum = true_light_color
    // else:

    // Attenuation
    vec3 light_color_sum = true_light_color;
    float dist = distance(u_light_position, v_vert.world_space_position);
    float attenuation_factor = 1.0 / (1.0 + linear_light_attenuation * dist + cubic_light_attenuation * pow(dist, 2.0));
    light_color_sum *= attenuation_factor;

    // Diffuse
    vec3 light_dir = normalize(u_light_position - world_space_position);
    float diffuse = max(dot(normal, light_dir), 0.0);
    diffuse_contrib = diffuse * light_color_sum;

    // Specular
    vec3 reflection_direction = reflect(-light_dir, normal);
    float specular = pow(max(dot(view_vector, reflection_direction), 0.0), shininess);
    specular_contrib = specular * u_specular_color;

    // Note: This treats the light like a spotlight
    // There could be a different function for each type of light
    vec3 spotlight_dir = normalize(u_light_position - world_space_position);
    float theta = dot(spotlight_dir, normalize(-u_light_direction));
    float epsilon = u_light_cutoff - u_light_outer_cutoff;
    float intensity = clamp((theta - u_light_outer_cutoff) / epsilon, 0.0, 1.0);

    diffuse_contrib *= intensity;
    specular_contrib *= intensity;
}

vec4 sum_lighting()
{
    vec4 out_color = vec4(0.0);
    vec3 diffuse_sum = vec3(0.0);
    vec3 specular_sum = vec3(0.0);

    vec3 normal = v_vert.world_space_normal;
    vec3 view_vector = normalize(u_camera_position - v_vert.world_space_position);

    // Flip the backface normals
    if (!gl_FrontFacing) {
      normal = -normal;
    }

    // Add diffuse and specular contributions for each light
    for (int i = 0; i < num_lights; i++)
    {
        vec3 diffuse_contrib = vec3(0.0);
        vec3 specular_contrib = vec3(0.0);

        calculatePhong(diffuse_contrib,
                       specular_contrib,
                       i,
                       v_vert.world_space_position,
                       normal,
                       u_shadow_strength,
                       u_shadow_color,
                       view_vector,
            u_shininess);

        diffuse_sum += diffuse_contrib;
        specular_sum += specular_contrib;
    }
    
    // Final diffuse contribution
    diffuse_sum *= u_diffuse_color.rgb * u_vertex_color.rgb * vec3(texture(u_diffuse_texture, v_vert.uv));
    out_color.rgb += diffuse_sum;

    // Final specular contribution
    specular_sum *= u_specular_color * vec3(texture(u_specular_texture, v_vert.uv));
    out_color.rgb += specular_sum;

    // If using ambient light:
    //   (for every ambient light)
    //     out_color += ambientColor * materialAmbientColor * objectColor

    // If using emit color
    //   out_color += emitColor * emitMap

    // If using constant color
    //   out_color += constantColor * objectColor

    // If light multiplies alpha:
    // calculate lightness (overall value of how much light is affecting the surface) and use in alpha calculation
    // vec3 lightness = out_color.r * 0.3 + out_color.g * 0.6 + out_color.b * 0.1;

    // If using color, environment or darkness maps or fog
    // out_color *= ColorMap
    // out_color += EnvironementMap * EnvironmentMapColor
    // out_color = mix(FogColor * FogMapColor, out_color, FogFactor)
    // out_color = mix(darknessEmitMapColor * darknessEmitColor, out_color, lightness)

    // Alpha calculation
    // If using alpha map or color map with alpha multiply those values here
    float alpha =  u_vertex_color.a * u_diffuse_color.a;
    out_color.rgb *= alpha;
    out_color.a = alpha;

    // If using a diffuse map:
    // out_color *= diffuseMap

    return out_color;
}

void main()
{
    frag_color = sum_lighting();
}
