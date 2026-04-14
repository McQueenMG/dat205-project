#pragma once

#include <glm/glm.hpp>

namespace vk
{
    class Scene
    {
    public:
        void initialize();
        void shutdown();

        // Camera
        glm::vec3 cameraPosition;
        glm::vec3 cameraDirection;

        // Lighting
        glm::vec3 lightPosition;
        glm::vec3 lightColor;

        // TODO: Models, transforms
    };
}
