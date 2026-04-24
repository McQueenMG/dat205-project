#include "simple_pipeline.h"
#include "../util.h"
#include <stdexcept>
#include <array>
#include <vector>

namespace vk
{
	void SimplePipeline::initialize(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout)
	{

		auto vertShaderCode = vk::readFile("vk_shaders/simple.vert.spv");
		auto fragShaderCode = vk::readFile("vk_shaders/simple.frag.spv");

		vertShaderModule = vk::createShaderModule(vertShaderCode, device);
		fragShaderModule = vk::createShaderModule(fragShaderCode, device);

		auto stages = getShaderStages();

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates    = dynamicStates.data();

		// Vertex layout: position only (vec3)
		VkVertexInputBindingDescription bindingDesc{};
		bindingDesc.binding   = 0;
		bindingDesc.stride    = sizeof(float) * 3;
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription attrDesc{};
		attrDesc.binding  = 0;
		attrDesc.location = 0;
		attrDesc.format   = VK_FORMAT_R32G32B32_SFLOAT;
		attrDesc.offset   = 0;

		VkPipelineVertexInputStateCreateInfo vertexInput{};
		vertexInput.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInput.vertexBindingDescriptionCount   = 1;
		vertexInput.pVertexBindingDescriptions      = &bindingDesc;
		vertexInput.vertexAttributeDescriptionCount = 1;
		vertexInput.pVertexAttributeDescriptions    = &attrDesc;

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
			throw std::runtime_error("Failed to create simple graphics pipeline.");
		}
	}

	void SimplePipeline::shutdown(VkDevice device)
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

	std::array<VkPipelineShaderStageCreateInfo, 2> SimplePipeline::getShaderStages() const
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