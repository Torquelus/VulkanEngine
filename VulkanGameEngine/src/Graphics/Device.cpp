#include "Device.h"

#include <iostream>
#include <optional>
#include <stdexcept>

// Constructore=
Device::Device(const Instance* instance, const PhysicalDevice* physicalDevice, Surface* surface)
: m_Instance(instance), m_PhysicalDevice(physicalDevice), m_Surface(surface) {
	CreateQueueIndices();
	CreateLogicalDevice();
}

// Destructor
Device::~Device(){
	// Destroy device
	vkDestroyDevice(m_Device, nullptr);
}

// Fill in queue family indices
void Device::CreateQueueIndices(){
	// Query queue family properties
	uint32_t deviceQueueFamilyPropertyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice->GetPhysicalDevice(), &deviceQueueFamilyPropertyCount, nullptr);
	std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice->GetPhysicalDevice(), &deviceQueueFamilyPropertyCount, deviceQueueFamilyProperties.data());
	
	// Create optional variables for each queue family type
	std::optional<uint32_t> graphicsFamily, presentFamily, computeFamily, transferFamily;

	// Loop through all queue family properties
	for (uint32_t i = 0; i < deviceQueueFamilyPropertyCount; i++) {
		// Check for graphics support
		if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsFamily = i;
			m_GraphicsFamily = i;
			m_SupportedQueues |= VK_QUEUE_GRAPHICS_BIT;
		}

		// Check for presentation support
		VkBool32 presentSupport;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice->GetPhysicalDevice(), i, m_Surface->GetSurface(), &presentSupport);
		
		if (deviceQueueFamilyProperties[i].queueCount > 0 && presentSupport) {
			presentFamily = i;
			m_PresentFamily = i;
		}

		// Check for compute support
		if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			computeFamily = i;
			m_ComputeFamily = i;
			m_SupportedQueues |= VK_QUEUE_COMPUTE_BIT;
		}

		// Check for transfer support
		if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			transferFamily = i;
			m_TransferFamily = i;
			m_SupportedQueues |= VK_QUEUE_TRANSFER_BIT;
		}

		// Break if all found
		if (graphicsFamily && presentFamily && computeFamily && transferFamily) {
			break;
		}

	}

	// Throw error if graphics family not supported
	if (!graphicsFamily) {
		throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT!");
	}
}

// Create logical device with available queue families
void Device::CreateLogicalDevice(){
	// Get physical device features
	auto physicalDeviceFeatures = m_PhysicalDevice->GetFeatures();

	// Create info and priorities variables
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriorities[1] = { 0.0f };

	// Check if graphics queue is supported and enable it
	if (m_SupportedQueues & VK_QUEUE_GRAPHICS_BIT) {
		VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsFamily;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(graphicsQueueCreateInfo);
	}
	else {
		m_GraphicsFamily = VK_NULL_HANDLE;
	}

	// Check if compute queue is supported and enable it, else use graphics family
	if (m_SupportedQueues & VK_QUEUE_COMPUTE_BIT && m_ComputeFamily != m_GraphicsFamily) {
		VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
		computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		computeQueueCreateInfo.queueFamilyIndex = m_ComputeFamily;
		computeQueueCreateInfo.queueCount = 1;
		computeQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(computeQueueCreateInfo);
	}
	else {
		m_ComputeFamily = m_GraphicsFamily;
	}

	// Check if transfer queue is supported, else use graphics family
	if (m_SupportedQueues & VK_QUEUE_TRANSFER_BIT && m_TransferFamily != m_GraphicsFamily && m_TransferFamily != m_ComputeFamily) {
		VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
		transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transferQueueCreateInfo.queueFamilyIndex = m_TransferFamily;
		transferQueueCreateInfo.queueCount = 1;
		transferQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(transferQueueCreateInfo);
	}
	else {
		m_TransferFamily = m_GraphicsFamily;
	}

	// Enabled physical features
	VkPhysicalDeviceFeatures enabledFeatures = {};

	// Enable sampler anisotropy
	if (physicalDeviceFeatures.samplerAnisotropy) {
		enabledFeatures.samplerAnisotropy = VK_TRUE;
	}
	else {
		std::cout << "Selected GPU does not support sampler anisotropy!" << std::endl;
	}

	// Logical device create info
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	// Check if validation layers should be enabled
	if (m_Instance->GetEnableValidationLayers()) {
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Instance::m_ValidationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = Instance::m_ValidationLayers.data();
	}
	else {
		deviceCreateInfo.enabledLayerCount = 0;
	}
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(Instance::m_DeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = Instance::m_DeviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	// Create logical device
	if (vkCreateDevice(m_PhysicalDevice->GetPhysicalDevice(), &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create logical device!");
	}

	// Get queues
	vkGetDeviceQueue(m_Device, m_GraphicsFamily, 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_Device, m_PresentFamily, 0, &m_PresentQueue);
	vkGetDeviceQueue(m_Device, m_ComputeFamily, 0, &m_ComputeQueue);
	vkGetDeviceQueue(m_Device, m_TransferFamily, 0, &m_TransferQueue);

}
