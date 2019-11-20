#include "Surface.h"

#include <stdexcept>

// Constructor
Surface::Surface(const Instance* instance, const PhysicalDevice* physicalDevice, Window* window)
: m_Instance(instance), m_PhysicalDevice(physicalDevice), m_Window(window) {
	// Create window surface
	if (m_Window->CreateSurface(m_Instance->GetInstance(), nullptr, &m_Surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}

	// Get surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice->GetPhysicalDevice(), m_Surface, &m_Capabilities);
	
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
			break;
		}
	}
	if (!m_Format.format) {
		m_Format = formats[0];
	}

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
			break;
		}
	}
	if (!m_PresentMode) {
		m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	}
}

// Destructor
Surface::~Surface() {

}
