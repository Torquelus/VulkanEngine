#pragma once

#include "Device.h"
#include "PhysicalDevice.h"

class CommandPool {
public:
	CommandPool(Device* device, PhysicalDevice* physicalDevice);	// Constructor
	~CommandPool();	// Destructor

	// GETTERS
	const VkCommandPool GetCommandPool() const { return m_CommandPool; }
private:
	// VARIABLES
	Device* m_Device;					// Vulkan device

	VkCommandPool m_CommandPool;		// Vulkan command pool
};