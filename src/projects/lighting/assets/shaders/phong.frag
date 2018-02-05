#version 440 core

// Comments show where you could use the following features:
// shadows, ambient lights, emit and constant colors, light multiplying alpha, fog, "darkness emit"
// and the following maps:
// projection, environment, alpha, color

const int numLights = 1;

uniform vec4 uVertexColor;
uniform vec3 uCameraPos;

// Light properties
uniform vec3 uLightColor;
uniform vec3 uLightPos;
const float linearLightAttenuation = 0.09;
const float cubicLightAttenuation = 0.032;

// Spotlight properties
uniform vec3 uLightDirection;
uniform float uLightCutoff;
uniform float uLightOuterCutoff;

// Material uniforms
uniform vec4 uDiffuseColor;
uniform vec4 uAmbientColor;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform float uShadowStrength;
uniform vec3 uShadowColor;
uniform sampler2D uDiffuseTexture;
uniform sampler2D uSpecularTexture;

in Vertex {
    vec3 worldSpacePos;
    vec3 worldSpaceNorm;
    vec2 uv;
} vVert;

layout(location = 0) out vec4 fragColor;

void calculatePhong(inout vec3 diffuseContrib, inout vec3 specularContrib, int lightIndex, vec3 worldSpacePos,
                    vec3 normal, float shadowStrength, vec3 shadowColor, vec3 viewVec, float shininess)
{
    // Note: Use lightIndex to index into an array of lights any time uLightColor is used

    // If using a projection map:
    //   trueLightColor = uLightColor * projectionMapColor
    // else:
    vec3 trueLightColor = uLightColor;

    // If using shadows:
    //   if (in a shadow)
    //      lightColorSum = mix(trueLightColor, shadowColor, shadowStrength)
    //    else:
    //      lightColorSum = trueLightColor
    // else:

    // Attenuation
    vec3 lightColorSum = trueLightColor;
    float dist = distance(uLightPos, vVert.worldSpacePos);
    float attenuationFactor = 1.0 / (1.0 + linearLightAttenuation * dist + cubicLightAttenuation * pow(dist, 2.0));
    lightColorSum *= attenuationFactor;

    // Diffuse
    vec3 lightDir = normalize(uLightPos - worldSpacePos);
    float diffuse = max(dot(normal, lightDir), 0.0);
    diffuseContrib = diffuse * lightColorSum;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specular = pow(max(dot(viewVec, reflectDir), 0.0), shininess);
    specularContrib = specular * uSpecularColor;

    // Note: This treats the light like a spotlight
    // Should have a different function for each different light
    vec3 spotlightDir = normalize(uLightPos - worldSpacePos);
    float theta = dot(spotlightDir, normalize(-uLightDirection));
    float epsilon = uLightCutoff - uLightOuterCutoff;
    float intensity = clamp((theta - uLightOuterCutoff) / epsilon, 0.0, 1.0);

    diffuseContrib *= intensity;
    specularContrib *= intensity;
}

vec4 sumLighting()
{
    vec4 outColor = vec4(0.0);
    vec3 diffuseSum = vec3(0.0);
    vec3 specularSum = vec3(0.0);

    vec3 normal = vVert.worldSpaceNorm;
    vec3 viewVec = normalize(uCameraPos - vVert.worldSpacePos);

    // Flip the normals on backfaces
    if (!gl_FrontFacing) {
      normal = -normal;
    }

    // Add diffuse and specular contributions for each light
    for (int i = 0; i < numLights; i++)
    {
        vec3 diffuseContrib = vec3(0.0);
        vec3 specularContrib = vec3(0.0);

        calculatePhong(diffuseContrib,
                       specularContrib,
                       i,
                       vVert.worldSpacePos,
                       normal,
                       uShadowStrength,
                       uShadowColor,
                       viewVec,
            uShininess);

        diffuseSum += diffuseContrib;
        specularSum += specularContrib;
    }

    // Final diffuse contribution
    diffuseSum *= uDiffuseColor.rgb * uVertexColor.rgb * vec3(texture(uDiffuseTexture, vVert.uv));
    outColor.rgb += diffuseSum;

    // Final specular contribution
    specularSum *= uSpecularColor * vec3(texture(uSpecularTexture, vVert.uv));
    outColor.rgb += specularSum;

    // If using ambient light:
    //   (for every ambient light)
    //     outColor += ambientColor * materialAmbientColor * objectColor

    // If using emit color
    //   outColor += emitColor * emitMap

    // If using constant color
    //   outColor += constantColor * objectColor

    // If light multiplies alpha:
    // calculate lightness (overall value of how much light is affecting the surface) and use in alpha calculation
    // vec3 lightness = outColor.r * 0.3 + outColor.g * 0.6 + outColor.b * 0.1;

    // If using color, environment or darkness maps or fog
    // outColor *= ColorMap
    // outColor += EnvironementMap * EnvironmentMapColor
    // outColor = mix(FogColor * FogMapColor, outColor, FogFactor)
    // outColor = mix(darknessEmitMapColor * darknessEmitColor, outColor, lightness)

    // Alpha calculation
    // If using alpha map or color map with alpha multiply those values here
    float alpha =  uVertexColor.a * uDiffuseColor.a;
    outColor.rgb *= alpha;
    outColor.a = alpha;

    // If using a diffuse map:
    // outColor *= diffuseMap

    return outColor;
}

void main()
{
    fragColor = sumLighting();
}
