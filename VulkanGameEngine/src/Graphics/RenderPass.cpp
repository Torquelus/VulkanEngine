#include "RenderPass.h"

#include <stdexcept>

// Constructor
RenderPass::RenderPass(Device* device, Swapchain* swapchain)
: m_Device(device), m_Swapchain(swapchain) {
	// Temp vectors
	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkSubpassDescription> subpasses;

	// Colour attachment description
	VkAttachmentDescription colourAttachment = {};
	colourAttachment.format = m_Swapchain->GetImageFormat();
	colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachments.emplace_back(colourAttachment);

	// Colour attachment reference
	VkAttachmentReference colourAttachmentRef = {};
	colourAttachmentRef.attachment = 0;
	colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Create colour attachment subpass
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentRef;
	subpasses.emplace_back(subpass);

	// Dependency info
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Render pass creation info
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	// Create render pass
	if (vkCreateRenderPass(m_Device->GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create render pass!");
	}
}

// Destructor
RenderPass::~RenderPass(){
	vkDestroyRenderPass(m_Device->GetDevice(), m_RenderPass, nullptr);
}

// Begin render pass
void RenderPass::Begin(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer){
	// Create viewport
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_Swapchain->GetExtent().width;
	viewport.height = (float)m_Swapchain->GetExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	// Create scissor
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_Swapchain->GetExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// Starting a render pass
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_RenderPass;
	renderPassInfo.framebuffer = frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_Swapchain->GetExtent();

	// Clear value
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &m_ClearColour;

	// Begin render pass
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

}

// End render pass
void RenderPass::End(VkCommandBuffer commandBuffer){
	vkCmdEndRenderPass(commandBuffer);
}
