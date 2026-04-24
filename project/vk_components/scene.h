#pragma once

#include <glm/glm.hpp>

namespace vk
{
    class Scene
    {
    public:
        void initialize();
        void shutdown();

        glm::vec3 worldUp;

        // Camera
        glm::vec3 cameraPosition;
        glm::vec3 cameraDirection;
        float cameraSpeed;

        // Lighting
        glm::vec3 lightPosition;
        glm::vec3 lightColor;
        float lightIntensity;
        float environmentMultiplier;

        // Matrices
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;

        // Models
        glm::mat4 fighterModelMatrix;
        glm::mat4 landingPadModelMatrix;
        glm::mat4 iceCreamCarModelMatrix;

        void update(float currentTime, float width, float height);

        // TODO: Models, transforms
    };
}
