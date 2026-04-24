#include "shading_pipeline.h"
#include "../util.h"
#include <stdexcept>
#include <array>
#include <vector>

namespace vk
{
	void ShadingPipeline::initialize(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout)
	{

		auto vertShaderCode = vk::readFile("vk_shaders/shading.vert.spv");
		auto fragShaderCode = vk::readFile("vk_shaders/shading.frag.spv");

		vertShaderModule = vk::createShaderModule(vertShaderCode, device);
		fragShaderModule = vk::createShaderModule(fragShaderCode, device);

		auto stages = getShaderStages();

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// Vertex layout: position (vec3) + normal (vec3) + texCoord (vec2) interleaved
		VkVertexInputBindingDescription bindingDesc{};
		bindingDesc.binding   = 0;
		bindingDesc.stride    = sizeof(float) * 8; // 3+3+2
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::array<VkVertexInputAttributeDescription, 3> attrDescs{};
		// location 0 – position (vec3)
		attrDescs[0].binding  = 0;
		attrDescs[0].location = 0;
		attrDescs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
		attrDescs[0].offset   = 0;
		// location 1 – normal (vec3)
		attrDescs[1].binding  = 0;
		attrDescs[1].location = 1;
		attrDescs[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
		attrDescs[1].offset   = sizeof(float) * 3;
		// location 2 – texCoord (vec2)
		attrDescs[2].binding  = 0;
		attrDescs[2].location = 2;
		attrDescs[2].format   = VK_FORMAT_R32G32_SFLOAT;
		attrDescs[2].offset   = sizeof(float) * 6;

		VkPipelineVertexInputStateCreateInfo vertexInput{};
		vertexInput.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInput.vertexBindingDescriptionCount   = 1;
		vertexInput.pVertexBindingDescriptions      = &bindingDesc;
		vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs.size());
		vertexInput.pVertexAttributeDescriptions    = attrDescs.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount  = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable        = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth               = 1.0f;
		rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable         = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable  = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable   = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments    = &colorBlendAttachment;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount          = static_cast<uint32_t>(stages.size());
		pipelineInfo.pStages             = stages.data();
		pipelineInfo.pVertexInputState   = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState      = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState   = &multisampling;
		pipelineInfo.pColorBlendState    = &colorBlending;
		pipelineInfo.pDynamicState       = &dynamicState;
		pipelineInfo.layout              = pipelineLayout;
		pipelineInfo.renderPass          = renderPass;
		pipelineInfo.subpass             = 0;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shading graphics pipeline.");
		}
	}

	void ShadingPipeline::shutdown(VkDevice device)
	{
		if (pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device, pipeline, nullptr);
			pipeline = VK_NULL_HANDLE;
		}
		if (fragShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, fragShaderModule, nullptr);
			fragShaderModule = VK_NULL_HANDLE;
		}
		if (vertShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, vertShaderModule, nullptr);
			vertShaderModule = VK_NULL_HANDLE;
		}
	}

	std::array<VkPipelineShaderStageCreateInfo, 2> ShadingPipeline::getShaderStages() const
	{
		VkPipelineShaderStageCreateInfo vertStage{};
		vertStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
		vertStage.module = vertShaderModule;
		vertStage.pName  = "main";

		VkPipelineShaderStageCreateInfo fragStage{};
		fragStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStage.module = fragShaderModule;
		fragStage.pName  = "main";

		return {vertStage, fragStage};
	}
}