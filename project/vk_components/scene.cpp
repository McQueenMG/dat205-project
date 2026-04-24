#include "scene.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vk
{
    void Scene::initialize()
    {
        worldUp = glm::vec3(0.f, 1.f, 0.f);

        cameraPosition = glm::vec3(-70.f, 50.f, 70.f);
        cameraDirection = glm::normalize(glm::vec3(0.f) - cameraPosition);

        lightColor = glm::vec3(1.f);
        lightIntensity = 10000.f;
        environmentMultiplier = 1.5f;
        cameraSpeed = 50.0f;

        fighterModelMatrix = glm::translate(glm::vec3(0.f, 15.f, 0.f));
        landingPadModelMatrix = glm::mat4(1.f);
        iceCreamCarModelMatrix = glm::translate(glm::vec3(0.f, 2.f, 0.f));
    }

    void Scene::update(float currentTime, float width, float height)
    {
        projectionMatrix = glm::perspective(
            glm::radians(45.0f),
            width / height,
            5.0f,
            2000.0f);

        viewMatrix = glm::lookAt(
            cameraPosition,
            cameraPosition + cameraDirection,
            worldUp);

        glm::vec4 lightStart(40.f, 40.f, 0.f, 1.f);

        lightPosition =
            glm::vec3(glm::rotate(glm::mat4(1.f), currentTime, worldUp) * lightStart);
    }

    void Scene::shutdown()
    {
    }
}