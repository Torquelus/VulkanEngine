#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "Instance.h"

class PhysicalDevice {
public:

	PhysicalDevice(const Instance* instance);	// Constructor
	~PhysicalDevice();	// Destructor

	// GETTERS
	const VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
	const VkPhysicalDeviceProperties GetProperties() const { return m_Properties; }
	const VkPhysicalDeviceFeatures GetFeatures() const { return m_Features; }
	const VkPhysicalDeviceMemoryProperties GetMemoryProperties() const { return m_MemoryProperties; }
	const VkSampleCountFlagBits GetMsaaSamples() const { return m_MsaaSamples; }

private:
	// VARIABLES
	const Instance* m_Instance;	// Vulkan instance object

	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;	// Vulkan physical device
	VkPhysicalDeviceProperties m_Properties = {};		// Vulkan physical device properties
	VkPhysicalDeviceFeatures m_Features = {};			// Vulkan physical device features
	VkPhysicalDeviceMemoryProperties m_MemoryProperties = {};	// Vulkan physical device memory properties
	VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;// MSAA sample count

	// FUNCTIONS
	void PickPhysicalDevice();					// Function that picks physical device
	int RateDevice(VkPhysicalDevice device);	// Function that rates all physical devices and returns score
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);	// Check extension support on physical device
};

