#pragma once

#include <vulkan/vulkan.h>
#include "vk_swapchain.h"

namespace vk
{
    class Context;
    class Scene;

    class Renderer
    {
    public:
        void initialize(const Context &context);
        void shutdown(const Context &context);

        void render(const Scene &scene);

    private:
        Swapchain swapchain;
    };
}
