#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "PhysicalDevice.h"

class Graphics;

class Buffer {
public:
	Buffer(Device* device, PhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const void *data);	// Constructor
	~Buffer();	// Destructor

	// FUNCTIONS
	void Bind(VkCommandBuffer commandBuffer);	// Bind buffer
	void Draw(VkCommandBuffer commandBuffer);	// Draw buffer
	void MapMemory(void** data) const;	// Map data to memory
	void UnmapMemory() const;			// Unmap data from memory

	// GETTERS
	const VkDeviceSize GetSize() const { return m_Size; }
	const VkBuffer GetBuffer() const { return m_Buffer; }
	const VkDeviceMemory GetBufferMemory() const { return m_BufferMemory; }
private:
	// VARIABLES
	Device* m_Device;
	PhysicalDevice* m_PhysicalDevice;

	VkDeviceSize m_Size;				// Buffer memory size
	VkBuffer m_Buffer = VK_NULL_HANDLE; // Vulkan buffer
	VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;	// Buffer memory

	// FUNCTIONS
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};