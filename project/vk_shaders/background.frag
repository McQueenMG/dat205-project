#version 450

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 fragmentColor;

layout(set = 0, binding = 6) uniform sampler2D environmentMap;

layout(set = 0, binding = 9) uniform BackgroundUBO
{
    mat4 inv_PV;
    vec3 camera_pos;
    float environment_multiplier;
};

#define PI 3.14159265359

void main()
{
    vec4 pixel_world_pos = inv_PV * vec4(texCoord * 2.0 - 1.0, 1.0, 1.0);
    pixel_world_pos = (1.0 / pixel_world_pos.w) * pixel_world_pos;

    vec3 dir = normalize(pixel_world_pos.xyz - camera_pos);

    float theta = acos(max(-1.0, min(1.0, dir.y)));
    float phi = atan(dir.z, dir.x);
    if (phi < 0.0)
        phi = phi + 2.0 * PI;

    vec2 lookup = vec2(phi / (2.0 * PI), theta / PI);
    fragmentColor = environment_multiplier * texture(environmentMap, lookup);
}