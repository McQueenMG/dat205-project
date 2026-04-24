#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "pipelines/background_pipeline.h"
#include "pipelines/shading_pipeline.h"
#include "pipelines/simple_pipeline.h"

namespace vk
{
    class Context;

    class Pipeline
    {
    public:
        void initialize(VkDevice device, VkRenderPass renderPass, VkExtent2D extent);
        void shutdown(VkDevice device);

        VkPipelineLayout getBackgroundPipelineLayout() const { return backgroundPipelineLayout; }
        VkPipelineLayout getShadingPipelineLayout()    const { return shadingPipelineLayout; }
        VkPipelineLayout getSimplePipelineLayout()     const { return simplePipelineLayout; }

        VkPipeline getBackgroundPipeline() const { return backgroundPipeline.getPipeline(); }
        VkPipeline getShadingPipeline()    const { return shadingPipeline.getPipeline(); }
        VkPipeline getSimplePipeline()     const { return simplePipeline.getPipeline(); }

    private:
        // Each sub-pipeline gets its own layout + descriptor set layout
        BackgroundPipeline backgroundPipeline;
        ShadingPipeline    shadingPipeline;
        SimplePipeline     simplePipeline;

        VkDescriptorSetLayout backgroundDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout shadingDescriptorSetLayout    = VK_NULL_HANDLE;
        VkDescriptorSetLayout simpleDescriptorSetLayout     = VK_NULL_HANDLE;

        VkPipelineLayout backgroundPipelineLayout = VK_NULL_HANDLE;
        VkPipelineLayout shadingPipelineLayout    = VK_NULL_HANDLE;
        VkPipelineLayout simplePipelineLayout     = VK_NULL_HANDLE;

        void createGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkExtent2D extent);
    };
}