#include "Framebuffers.h"

#include <stdexcept>

// Constructor
Framebuffers::Framebuffers(Device* device, RenderPass* renderPass, Swapchain* swapchain)
: m_Device(device), m_RenderPass(renderPass), m_Swapchain(swapchain) {
	// Resize framebuffers to fit all images
	m_Framebuffers.resize(m_Swapchain->GetImageCount());

	// Loop through swapchain image views
	for (size_t i = 0; i < m_Swapchain->GetImageCount(); i++) {
		// Get image view
		VkImageView imageView = m_Swapchain->GetImageViews()[i];
		VkImageView attachments[] = { imageView };

		// Framebuffer creation info
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass->GetRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Swapchain->GetExtent().width;
		framebufferInfo.height = m_Swapchain->GetExtent().height;
		framebufferInfo.layers = 1;

		// Create framebuffer
		if (vkCreateFramebuffer(m_Device->GetDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Unable to create framebuffer!");
		}
	}
}

// Destructor
Framebuffers::~Framebuffers(){
	// Destroy all framebuffers
	for (auto framebuffer : m_Framebuffers) {
		vkDestroyFramebuffer(m_Device->GetDevice(), framebuffer, nullptr);
	}
}
