#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class Instance {
public:
	Instance();		// Constructor
	~Instance();	// Destructor

	static const std::vector<const char*> m_ValidationLayers;
	static const std::vector<const char*> m_DeviceExtensions;

	// GETTERS
	const VkInstance GetInstance() const { return m_Instance; }
	const bool GetEnableValidationLayers() const { return m_EnableValidationLayers; }
private:
	// VARIABLES
	VkInstance m_Instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

	// DEBUG
	// Debug layers
	#ifdef NDEBUG
		const bool m_EnableValidationLayers = false;
	#else
		const bool m_EnableValidationLayers = true;
	#endif

	// FUNCTIONS
	bool CheckValidationLayerSupport();	// Check if requested validation layers are supported
	std::vector<const char*> GetRequiredExtensions();	// Return required extensions
	
	// DEBUG LAYER FUNCTIONS
	void SetupDebugMessenger();			// Set up debug messenger
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
};