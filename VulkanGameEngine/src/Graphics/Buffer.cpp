#include "Buffer.h"

#include "Graphics.h"

#include <array>
#include <iostream>
#include <stdexcept>

// Constructor
Buffer::Buffer(Device* device, PhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const void* data)
: m_Size(size), m_Device(device), m_PhysicalDevice(physicalDevice) {
	// Query queue families
	auto graphicsFamily = m_Device->GetGraphicsFamily();
	auto presentFamily = m_Device->GetPresentFamily();
	auto computeFamily = m_Device->GetComputeFamily();
	std::array<uint32_t, 3> queueFamilies = { graphicsFamily, presentFamily, computeFamily };

	// Buffer create info
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());
	bufferInfo.pQueueFamilyIndices = queueFamilies.data();

	// Create buffer
	if (vkCreateBuffer(m_Device->GetDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create buffer!");
	}

	// Get memory requirements
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device->GetDevice(), m_Buffer, &memRequirements);

	// Memory allocation info
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	// Allocate memory
	if (vkAllocateMemory(m_Device->GetDevice(), &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Unable to allocate buffer memory!");
	}

	// Check if data has been passed, copy the data
	void* mapped;
	MapMemory(&mapped);
	std::memcpy(mapped, data, size);
	UnmapMemory();

	// Bind buffer to memory
	vkBindBufferMemory(m_Device->GetDevice(), m_Buffer, m_BufferMemory, 0);
}

// Destructor
Buffer::~Buffer(){
	// Destroy buffer
	std::cout << "Destroyed Buffer" << std::endl;
	vkDestroyBuffer(m_Device->GetDevice(), m_Buffer, nullptr);
	vkFreeMemory(m_Device->GetDevice(), m_BufferMemory, nullptr);
}

// Bind buffer
void Buffer::Bind(VkCommandBuffer commandBuffer){
	VkBuffer buffers[] = { m_Buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

// Draw buffer
void Buffer::Draw(VkCommandBuffer commandBuffer){
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

// Map data to memory
void Buffer::MapMemory(void** data) const{
	vkMapMemory(m_Device->GetDevice(), m_BufferMemory, 0, m_Size, 0, data);
}

// Unmap data from memory
void Buffer::UnmapMemory() const{
	vkUnmapMemory(m_Device->GetDevice(), m_BufferMemory);
}

uint32_t Buffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
	// Query physical device properties
	auto memProperties = m_PhysicalDevice->GetMemoryProperties();

	// Loop through memory types
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	// If nothing found throw error
	throw std::runtime_error("Unable to find suitable memory type!");
}
