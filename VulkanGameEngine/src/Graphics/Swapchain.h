#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "Device.h"
#include "PhysicalDevice.h"
#include "Surface.h"

// Struct for swap chain support details
struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> format;
	std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain {
public:
	Swapchain(Device* device, PhysicalDevice* physicalDevice, Surface* surface, Window* window, VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);	// Constructor
	~Swapchain();	// Destructor

	// FUNCTIONS
	VkResult AcquireNextImage(const VkSemaphore& presentCompleteSemaphore);	// Acquire next image in swapchain
	VkResult QueuePresent(const VkQueue& presentQueue, const VkSemaphore& waitSemaphore);	// Present swapchain image to queue

	// GETTERS
	VkExtent2D GetExtent() { return m_Extent; }
	VkPresentModeKHR GetPresentMode() { return m_PresentMode; }
	const uint32_t GetImageCount() const { return m_ImageCount; }
	const uint32_t GetActiveImageIndex() const { return m_ActiveImageIndex; }
	const std::vector<VkImageView> GetImageViews() const { return m_ImageViews; }
	const VkFormat GetImageFormat() const { return m_ImageFormat; }
	const VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }
private:
	// VARIABLES
	Device* m_Device;								// Vulkan logical device object
	PhysicalDevice* m_PhysicalDevice;				// Vulkan physical device object
	Surface* m_Surface;								// Vulkan surface object

	VkExtent2D m_Extent;							// Swapchain extent
	VkPresentModeKHR m_PresentMode;					// Swapchain present mode

	uint32_t m_ImageCount = 0;						// Amount of images in swapchain
	uint32_t m_ActiveImageIndex = 0;				// Index of active image in swapchain
	std::vector<VkImage> m_Images;					// Swapchain images
	std::vector<VkImageView> m_ImageViews;			// Swapchain image views
	VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;	// Vulkan swapchain
	VkFormat m_ImageFormat;							// Vulkan image format

};