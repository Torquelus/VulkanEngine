#pragma once

#include <vector>

#include "Device.h"
#include "RenderPass.h"
#include "Swapchain.h"

class Framebuffers {
public:
	Framebuffers(Device* device, RenderPass* renderPass, Swapchain* swapchain);	// Constructor
	~Framebuffers();	// Destructor

	// GETTERS
	const std::vector<VkFramebuffer> GetFramebuffers() const { return m_Framebuffers; }
private:
	// VARIABLES
	Device* m_Device;			// Vulkan device
	RenderPass* m_RenderPass;	// Vulkan render pass
	Swapchain* m_Swapchain;		// Vulkan swapchain

	std::vector<VkFramebuffer> m_Framebuffers;	// Framebuffers for swapchain
};