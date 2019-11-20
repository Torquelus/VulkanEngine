#include "CommandBuffer.h"

#include <stdexcept>

// Constructor
CommandBuffer::CommandBuffer(Device* device, CommandPool* commandPool)
: m_Device(device), m_CommandPool(commandPool) {
	// Buffer allocation info
	auto commandPoolUse = m_CommandPool->GetCommandPool();
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPoolUse;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	// Create command buffer
	if (vkAllocateCommandBuffers(m_Device->GetDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Unable to allocate command buffer!");
	}

	// Start not running
	m_Running = false;
}

// Destructor
CommandBuffer::~CommandBuffer(){
	// Free command buffer
	vkFreeCommandBuffers(m_Device->GetDevice(), m_CommandPool->GetCommandPool(), 1, &m_CommandBuffer);
}

// Begin command buffer
void CommandBuffer::Begin(VkCommandBufferUsageFlags usage){
	// Return if already running
	if (m_Running) return;

	// Begin command buffer
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = usage;
	
	// Begin command buffer
	if (vkBeginCommandBuffer(m_CommandBuffer,  &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Unable to begin command buffer!");
	}

	// Set bool for running to true
	m_Running = true;
}

// End command buffer
void CommandBuffer::End(){
	// If already not running, return
	if (!m_Running) return;

	// End command buffer
	if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Unable to end command buffer!");
	}

	// Set bool for running to false
	m_Running = false;
}

// Submit command buffer
void CommandBuffer::Submit(const VkSemaphore& waitSemaphore, const VkSemaphore& signalSemaphore, VkFence currentFence){
	// End if currently running
	if (m_Running) End();

	// Create submit info
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffer;

	// Check if wait semaphore exists and add it
	if (waitSemaphore != VK_NULL_HANDLE) {
		VkSemaphore waitSemaphores[] = { waitSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
	}

	// Check if signal semaphore exists and add it
	if (signalSemaphore != VK_NULL_HANDLE) {
		VkSemaphore signalSemaphores[] = { signalSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
	}

	// Check if fence exists and reset if it does
	if (currentFence != VK_NULL_HANDLE) {
		vkResetFences(m_Device->GetDevice(), 1, &currentFence);
	}

	// Submit to queue
	if (vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, currentFence) != VK_SUCCESS) {
		throw std::runtime_error("Unable to submit command buffer!");
	}
}
