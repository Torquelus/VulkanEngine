#include "CommandPool.h"

#include <stdexcept>

// Constructor
CommandPool::CommandPool(Device* device, PhysicalDevice* physicalDevice)
: m_Device(device) {
	// Command pool creation info
	auto graphicsFamily = m_Device->GetGraphicsFamily();
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicsFamily;
	poolInfo.flags = 0;

	// Create command pool
	if (vkCreateCommandPool(m_Device->GetDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create command pool!");
	}
}

// Destructor
CommandPool::~CommandPool(){
	// Destroy command pool
	vkDestroyCommandPool(m_Device->GetDevice(), m_CommandPool, nullptr);
}
