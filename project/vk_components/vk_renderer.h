#pragma once

#include <vulkan/vulkan.h>
#include "vk_swapchain.h"
#include "vk_pipeline.h"
#include "vk_context.h"


namespace vk
{
    class Context;
    class Scene;

    class Renderer
    {
    public:
        void initialize(const Context &context);
        void shutdown(VkDevice device);

        void render(const Scene &scene);
        VkRenderPass getRenderPass() const { return renderPass; }

    private:
        VkRenderPass renderPass = VK_NULL_HANDLE;
        Swapchain swapchain;
        Pipeline pipeline;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        void createRenderPass(VkDevice device, VkFormat swapChainImageFormat);
        void createFrameBuffers(VkDevice device, const std::vector<VkImageView> &imageViews, VkExtent2D extent);
        void createCommandPool(VkDevice device, vk::QueueFamilyIndices queueFamilyIndices);
        void createCommandBuffers(VkDevice device);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    };
}
