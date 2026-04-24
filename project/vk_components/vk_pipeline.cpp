#include "vk_context.h"
#include "vk_pipeline.h"
#include <stdexcept>
#include <array>

namespace vk
{
    void Pipeline::initialize(VkDevice device, VkRenderPass renderPass, VkExtent2D extent)
    {
        createGraphicsPipeline(device, renderPass, extent);
    }

    // ---------------------------------------------------------------------------
    // Helper – create a VkDescriptorSetLayout from a list of bindings
    // ---------------------------------------------------------------------------
    static VkDescriptorSetLayout createDescriptorSetLayout(
        VkDevice device,
        const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    {
        VkDescriptorSetLayoutCreateInfo info{};
        info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = static_cast<uint32_t>(bindings.size());
        info.pBindings    = bindings.data();

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        if (vkCreateDescriptorSetLayout(device, &info, nullptr, &layout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout.");
        return layout;
    }

    void Pipeline::createGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkExtent2D extent)
    {
        // -----------------------------------------------------------------------
        // Background pipeline layout
        //   binding 6  – environmentMap          (combined image sampler, frag)
        //   binding 9  – BackgroundUBO           (uniform buffer,          frag)
        // -----------------------------------------------------------------------
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings = {
                // binding 6 – environmentMap
                { 6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
                // binding 9 – BackgroundUBO
                { 9, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
            };
            backgroundDescriptorSetLayout = createDescriptorSetLayout(device, bindings);

            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts    = &backgroundDescriptorSetLayout;

            if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &backgroundPipelineLayout) != VK_SUCCESS)
                throw std::runtime_error("Failed to create background pipeline layout.");

            backgroundPipeline.initialize(device, renderPass, backgroundPipelineLayout);
        }

        // -----------------------------------------------------------------------
        // Shading pipeline layout
        //   binding 0  – colorMap               (combined image sampler, frag)
        //   binding 5  – emissiveMap             (combined image sampler, frag)
        //   binding 6  – environmentMap          (combined image sampler, frag)
        //   binding 7  – irradianceMap           (combined image sampler, frag)
        //   binding 8  – reflectionMap           (combined image sampler, frag)
        //   binding 0  – ShadingVertexUBO        (uniform buffer,          vert)  -- CONFLICT with colorMap!
        //
        // Note: binding 0 is claimed by both ShadingVertexUBO (vert) and colorMap
        // (frag). We resolve this by giving them different stage flags on the same
        // binding slot, which is valid in Vulkan when the descriptor type differs.
        // Since they ARE different types (UBO vs sampler) we must use separate
        // bindings. Move ShadingVertexUBO to binding 10 here and in shading.vert.
        //
        //   binding 0  – colorMap               (combined image sampler, frag)
        //   binding 5  – emissiveMap             (combined image sampler, frag)
        //   binding 6  – environmentMap          (combined image sampler, frag)
        //   binding 7  – irradianceMap           (combined image sampler, frag)
        //   binding 8  – reflectionMap           (combined image sampler, frag)
        //   binding 9  – ShadingMaterialUBO      (uniform buffer,         frag)
        //   binding 10 – ShadingVertexUBO        (uniform buffer,         vert)
        // -----------------------------------------------------------------------
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings = {
                { 0,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
                { 5,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
                { 6,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
                { 7,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
                { 8,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
                { 9,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
                { 10, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_VERTEX_BIT,   nullptr },
                { 11, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
            };
            shadingDescriptorSetLayout = createDescriptorSetLayout(device, bindings);

            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts    = &shadingDescriptorSetLayout;

            if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &shadingPipelineLayout) != VK_SUCCESS)
                throw std::runtime_error("Failed to create shading pipeline layout.");

            shadingPipeline.initialize(device, renderPass, shadingPipelineLayout);
        }

        // -----------------------------------------------------------------------
        // Simple pipeline layout
        //   binding 0 – SimpleVertexUBO          (uniform buffer, vert)
        //   binding 1 – SimpleFragmentUBO        (uniform buffer, frag)
        // -----------------------------------------------------------------------
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings = {
                { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT,   nullptr }, // SimpleVertexUBO
                { 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // SimpleFragmentUBO
            };
            simpleDescriptorSetLayout = createDescriptorSetLayout(device, bindings);

            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts    = &simpleDescriptorSetLayout;

            if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &simplePipelineLayout) != VK_SUCCESS)
                throw std::runtime_error("Failed to create simple pipeline layout.");

            simplePipeline.initialize(device, renderPass, simplePipelineLayout);
        }
    }

    void Pipeline::shutdown(VkDevice device)
    {
        simplePipeline.shutdown(device);
        shadingPipeline.shutdown(device);
        backgroundPipeline.shutdown(device);

        if (simplePipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, simplePipelineLayout, nullptr);
            simplePipelineLayout = VK_NULL_HANDLE;
        }
        if (shadingPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, shadingPipelineLayout, nullptr);
            shadingPipelineLayout = VK_NULL_HANDLE;
        }
        if (backgroundPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, backgroundPipelineLayout, nullptr);
            backgroundPipelineLayout = VK_NULL_HANDLE;
        }

        if (simpleDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, simpleDescriptorSetLayout, nullptr);
            simpleDescriptorSetLayout = VK_NULL_HANDLE;
        }
        if (shadingDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, shadingDescriptorSetLayout, nullptr);
            shadingDescriptorSetLayout = VK_NULL_HANDLE;
        }
        if (backgroundDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, backgroundDescriptorSetLayout, nullptr);
            backgroundDescriptorSetLayout = VK_NULL_HANDLE;
        }
    }
}