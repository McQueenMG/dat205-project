#pragma once

#include <array>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk
{
	class SimplePipeline
	{
	public:
		void initialize(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout);
		void shutdown(VkDevice device);
		std::array<VkPipelineShaderStageCreateInfo, 2> getShaderStages() const;
		VkPipeline getPipeline() const { return pipeline; }

	private:
		VkShaderModule vertShaderModule = VK_NULL_HANDLE;
		VkShaderModule fragShaderModule = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
	};
}
