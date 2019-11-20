#include "Swapchain.h"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <iostream>

// Constructor
Swapchain::Swapchain(Device* device, PhysicalDevice* physicalDevice, Surface* surface, Window* window, VkSwapchainKHR oldSwapchain)
: m_Device(device), m_PhysicalDevice(physicalDevice), m_Surface(surface) {
	// Get details
	auto capabilities = m_Surface->GetCapabilities();
	m_ImageFormat = m_Surface->GetFormat().format;

	// Get present mode
	m_PresentMode = m_Surface->GetPresentMode();

	// Get swapchain extent
	if (capabilities.currentExtent.width != UINT32_MAX) {
		m_Extent = capabilities.currentExtent;
	}
	else {
		uint32_t width = window->GetWidth();
		uint32_t height = window->GetHeight();
		m_Extent = { width, height};
	}

	// Clamp values
	m_Extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, m_Extent.width));
	m_Extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, m_Extent.height));
	
	// Query image count
	uint32_t imageCount = capabilities.minImageCount + 1;

	// Clamp to max image count
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	// Swap chain create info
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	auto surfaceNow = m_Surface->GetSurface();
	createInfo.surface = surfaceNow;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = m_ImageFormat;
	auto format = m_Surface->GetFormat();
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = m_Extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Queue sharing info
	auto graphicsFamily = m_Device->GetGraphicsFamily();
	auto presentFamily = m_Device->GetPresentFamily();

	if (graphicsFamily != presentFamily) {
		std::array<uint32_t, 2> queueFamily = { graphicsFamily, presentFamily };
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
		createInfo.pQueueFamilyIndices = queueFamily.data();
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	// Continue create info
	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = m_PresentMode;
	createInfo.clipped = VK_TRUE;

	// Create swapchain
	auto result = vkCreateSwapchainKHR(m_Device->GetDevice(), &createInfo, nullptr, &m_Swapchain);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create swap chain!");
	}

	// Get swapchain images
	vkGetSwapchainImagesKHR(m_Device->GetDevice(), m_Swapchain, &m_ImageCount, nullptr);
	m_Images.resize(m_ImageCount);
	vkGetSwapchainImagesKHR(m_Device->GetDevice(), m_Swapchain, &m_ImageCount, m_Images.data());

	// Create image views for swapchain
	m_ImageViews.resize(m_ImageCount);
	for (size_t i = 0; i < m_ImageCount; i++) {
		// Image view create info
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_Images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_ImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		// Create image view
		if (vkCreateImageView(m_Device->GetDevice(), &createInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Unable to create swapchain image views!");
		}
	}

}

// Destructor
Swapchain::~Swapchain(){
	// Destroy image views
	for (auto imageView : m_ImageViews) {
		vkDestroyImageView(m_Device->GetDevice(), imageView, nullptr);
	}
	
	// Destroy swapchain
	vkDestroySwapchainKHR(m_Device->GetDevice(), m_Swapchain, nullptr);
}

// Acquire next image in swapchain
VkResult Swapchain::AcquireNextImage(const VkSemaphore& presentCompleteSemaphore){
	// Get next image
	VkResult acquireResult = vkAcquireNextImageKHR(m_Device->GetDevice(), m_Swapchain, UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &m_ActiveImageIndex);

	// Return result of acquire
	return acquireResult;
}

// Present swapchain image to queue
VkResult Swapchain::QueuePresent(const VkQueue& presentQueue, const VkSemaphore& waitSemaphore){
	// Present info
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &waitSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain;
	presentInfo.pImageIndices = &m_ActiveImageIndex;
	
	// Return result of present
	return vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);
}
