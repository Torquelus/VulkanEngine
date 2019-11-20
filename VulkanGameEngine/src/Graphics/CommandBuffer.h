#pragma once

#include "CommandPool.h"
#include "Device.h"

class CommandBuffer {
public:
	CommandBuffer(Device* device, CommandPool* commandPool);	// Constructor
	~CommandBuffer();	// Destructor

	// FUNCTIONS
	void Begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);	// Begin command buffer
	void End();	// End command buffer
	void Submit(const VkSemaphore& waitSemaphore, const VkSemaphore& signalSemaphore, VkFence currentFence);	// Submit command buffer

	// GETTERS
	const VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }
	const bool IsRunning() const { return m_Running; }
private:
	// VARIABLES
	Device* m_Device;					// Vulkan device
	CommandPool* m_CommandPool;			// Vulkan command pool

	VkCommandBuffer m_CommandBuffer;	// Vulkan command buffer
	bool m_Running;						// Bool if command buffer is running
};