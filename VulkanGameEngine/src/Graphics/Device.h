#pragma once

#include <vulkan/vulkan.h>

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Surface.h"

class Device {
public:
	Device(const Instance* instance, const PhysicalDevice* physicalDevice, Surface* surface);	// Constructor
	~Device();	// Destructor

	// GETTERS
	const VkDevice GetDevice() const { return m_Device; }
	const VkPhysicalDeviceFeatures GetEnabledFeatures() const { return m_EnabledFeatures; }
	const VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
	const VkQueue GetPresentQueue() const { return m_PresentQueue; }
	const VkQueue GetComputeQueue() const { return m_ComputeQueue; }
	const VkQueue GetTransferQueue() const { return m_TransferQueue; }
	uint32_t GetGraphicsFamily() const { return m_GraphicsFamily; }
	uint32_t GetPresentFamily() const { return m_PresentFamily; }
	uint32_t GetComputeFamily() const { return m_ComputeFamily; }
	uint32_t GetTransferFamily() const { return m_TransferFamily; }
private:
	// VARIABLES
	const Instance* m_Instance;					// Vulkan instance object
	const PhysicalDevice* m_PhysicalDevice;		// Vulkan physical device object
	const Surface* m_Surface;					// Surface to render to

	VkDevice m_Device = VK_NULL_HANDLE;	// Vulkan logical device
	VkPhysicalDeviceFeatures m_EnabledFeatures = {};	// Enabled features

	VkQueueFlags m_SupportedQueues = {};		// List of supported queues
	uint32_t m_GraphicsFamily = 0;				// Graphics family
	uint32_t m_PresentFamily = 0;				// Present family
	uint32_t m_ComputeFamily = 0;				// Compute family
	uint32_t m_TransferFamily = 0;				// Transfer family

	VkQueue m_GraphicsQueue = VK_NULL_HANDLE;	// Vulkan graphics queue
	VkQueue m_PresentQueue = VK_NULL_HANDLE;	// Vulkan present queue
	VkQueue m_ComputeQueue = VK_NULL_HANDLE;	// Vulkan compute queue
	VkQueue m_TransferQueue = VK_NULL_HANDLE;	// Vulkan transfer queue

	// FUNCTIONS
	void CreateQueueIndices();					// Fill in queue family indices
	void CreateLogicalDevice();					// Create logical device with available queue families
};