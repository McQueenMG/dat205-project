#version 450

layout(location = 0) out vec4 fragmentColor;

layout(set = 0, binding = 0) uniform sampler2D colorMap;
layout(set = 0, binding = 5) uniform sampler2D emissiveMap;
layout(set = 0, binding = 6) uniform sampler2D environmentMap;
layout(set = 0, binding = 7) uniform sampler2D irradianceMap;
layout(set = 0, binding = 8) uniform sampler2D reflectionMap;

layout(set = 0, binding = 9) uniform ShadingMaterialUBO
{
    vec3 material_color;
    float material_reflectivity;
    float material_metalness;
    float material_fresnel;
    float material_shininess;
    float material_emission;
    int has_emission_texture;
    int has_color_texture;
};

layout(set = 0, binding = 11) uniform ShadingSceneUBO
{
    float environment_multiplier;
    vec3 point_light_color;
    float point_light_intensity_multiplier;
    mat4 viewInverse;
    vec3 viewSpaceLightPosition;
};

#define PI 3.14159265359

layout(location = 0) in vec2 texCoord;
layout(location = 1) in vec3 viewSpaceNormal;
layout(location = 2) in vec3 viewSpacePosition;

vec3 calculateDirectIllumiunation(vec3 wo, vec3 n)
{
    vec3 baseColor = material_color;
    if (has_color_texture == 1)
    {
        baseColor *= texture(colorMap, texCoord).rgb;
    }
    return baseColor;
}

vec3 calculateIndirectIllumination(vec3 wo, vec3 n)
{
    return vec3(0.0);
}

void main()
{
    vec3 wo = -normalize(viewSpacePosition);
    vec3 n = normalize(viewSpaceNormal);

    vec4 texel = vec4(material_color, 1.0);
    if (has_color_texture == 1)
    {
        texel *= texture(colorMap, texCoord);

        if (texel.a < 0.1)
            discard;
    }

    vec3 baseColor = texel.rgb;
    vec3 direct_illumination_term = baseColor;
    vec3 indirect_illumination_term = vec3(0.0);

    vec3 emission_term = material_emission * baseColor;
    if (has_emission_texture == 1)
    {
        emission_term = texture(emissiveMap, texCoord).xyz;
    }

    vec3 shading = direct_illumination_term + indirect_illumination_term + emission_term;
    fragmentColor = vec4(shading, 1.0);
}