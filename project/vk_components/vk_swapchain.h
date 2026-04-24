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
        void shutdown(VkDevice device);
        VkExtent2D getExtent() const { return swapChainExtent; }
        VkFormat getImageFormat() const { return swapChainImageFormat; }
        const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }

    private:
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D swapChainExtent = {0, 0};
        std::vector<VkImageView> swapChainImageViews;

        void createSwapChain(const Context &context);
        void createImageViews(VkDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D drawableExtent);
    
    
    };
}
