#include "scene.h"
#include <glm/gtx/transform.hpp>

namespace vk
{
    void Scene::initialize()
    {
        // Default camera
        cameraPosition = glm::vec3(-70.0f, 50.0f, 70.0f);
        cameraDirection = glm::normalize(glm::vec3(0.0f) - cameraPosition);

        // Default lighting
        lightPosition = glm::vec3(40.0f, 40.0f, 0.0f);
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

        // TODO: Load models
    }

    void Scene::shutdown()
    {
        // TODO: Free models
    }
}
