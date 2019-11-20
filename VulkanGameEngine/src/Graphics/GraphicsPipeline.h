#pragma once

#include "Device.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Swapchain.h"

#include <vector>

class GraphicsPipeline {
public:
	GraphicsPipeline(Device* device, Swapchain* swapchain, RenderPass* renderPass);	// Constructor
	~GraphicsPipeline();// Destructor
	
	// FUNCTIONS
	void Bind(VkCommandBuffer commandBuffer);	// Bind graphics pipeline to command buffer

	// GETTERS
	const VkPipeline GetGraphicsPipeline() const { return m_GraphicsPipeline; }
	const VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
private:
	// VARIABLES
	Device* m_Device;			// Vulkan device
	Swapchain* m_Swapchain;		// Vulkan swapchain
	RenderPass* m_RenderPass;	// Vulkan render pass

	VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;		// Vulkan graphics pipeline
	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;	// Vulkan pipeline layout

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages = {};	// Shader stages pipeline will use

	// FUNCTIONS
	void AddShader(Shader* shader);	// Add shader to pipeline shader stage
};