#pragma once
#include <glm/glm.hpp>

namespace vk
{
    struct alignas(16) ShadingSceneUBO
    {
        float environment_multiplier = 1.5f;

        alignas(16) glm::vec3 point_light_color = glm::vec3(1.f);

        float point_light_intensity_multiplier = 10000.f;

        glm::mat4 viewInverse = glm::mat4(1.f);

        alignas(16) glm::vec3 viewSpaceLightPosition = glm::vec3(0.f);
    };
    struct alignas(16) ShadingVertexUBO
    {
        glm::mat4 normalMatrix = glm::mat4(1.f);
        glm::mat4 modelViewMatrix = glm::mat4(1.f);
        glm::mat4 modelViewProjectionMatrix = glm::mat4(1.f);
    };
}