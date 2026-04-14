#pragma once

#include <vulkan/vulkan.h>

namespace vk
{
    class Context;

    class Swapchain
    {
    public:
        void initialize(const Context &context);
        void shutdown(const Context &context);

    private:
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    };
}
