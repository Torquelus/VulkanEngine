#pragma once

#include "Device.h"
#include "Swapchain.h"

#include <vector>

class RenderPass {
public:
	RenderPass(Device* device, Swapchain* swapchain);	// Constructor
	~RenderPass();	// Destructor

	// FUNCTIONS
	void Begin(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer);	// Begin render pass
	void End(VkCommandBuffer commandBuffer);	// End render pass

	// GETTERS
	VkRenderPass GetRenderPass() { return m_RenderPass; }
	
	// SETTERS
	void SetClearColour(VkClearValue clearColour) { m_ClearColour = clearColour; }
private:
	// VARIABLES
	Device* m_Device;
	Swapchain* m_Swapchain;	// Vulkan swapchain object

	VkRenderPass m_RenderPass = VK_NULL_HANDLE;	// Vulkan render pass
	VkClearValue m_ClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };	// Vulkan clear colour (black by def)
};