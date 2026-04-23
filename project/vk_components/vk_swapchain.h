#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vk
{
    class Context;

    class Swapchain
    {
    public:
        void initialize(const Context &context);
        void shutdown(const Context &context);

    private:
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D drawableExtent);
    };
}
