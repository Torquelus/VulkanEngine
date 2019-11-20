#pragma once

#include <vulkan/vulkan.h>

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Window.h"

class Surface {
public:
	Surface(const Instance* instance, const PhysicalDevice* physicalDevice, Window* window);
	~Surface();

	// GETTERS
	const VkSurfaceKHR GetSurface() const { return m_Surface; }
	VkSurfaceCapabilitiesKHR GetCapabilities() { 
		// Get surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice->GetPhysicalDevice(), m_Surface, &m_Capabilities);
		return m_Capabilities; 
	}
	VkSurfaceFormatKHR GetFormat() {
		// Query formats
		uint32_t formatCount;
		std::vector<VkSurfaceFormatKHR> formats;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice->GetPhysicalDevice(), m_Surface, &formatCount, nullptr);
		if (formatCount != 0) {
			formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice->GetPhysicalDevice(), m_Surface, &formatCount, formats.data());
		}

		// Select format by looping through available formats
		for (const auto& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_Format = format;
			}
		}
		if (!m_Format.format) {
			m_Format = formats[0];
		}
		return m_Format;
	}
	VkPresentModeKHR GetPresentMode() {
		// Query present modes
		uint32_t presentModeCount;
		std::vector<VkPresentModeKHR> presentModes;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice->GetPhysicalDevice(), m_Surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice->GetPhysicalDevice(), m_Surface, &presentModeCount, presentModes.data());
		}

		// Choose present mode
		for (const auto& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				m_PresentMode = presentMode;
			}
		}
		if (!m_PresentMode) {
			m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}
		return m_PresentMode;
	}
private:
	// VARIABLES
	const Instance* m_Instance;						// Vulkan instance object
	const PhysicalDevice* m_PhysicalDevice;			// Vulkan physical device object
	Window* m_Window;								// Window object

	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;		// Vulkan surface
	VkSurfaceCapabilitiesKHR m_Capabilities = {};	// Surface capabilities
	VkSurfaceFormatKHR m_Format = {};				// Surface format
	VkPresentModeKHR m_PresentMode = {};			// Surface present mode
};