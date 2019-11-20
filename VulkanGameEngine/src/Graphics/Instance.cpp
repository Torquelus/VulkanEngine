#include "Instance.h"

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

// Validation layers to use
const std::vector<const char*> Instance::m_ValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

// Device extensions to enable
const std::vector<const char*> Instance::m_DeviceExtensions = { 
	VK_KHR_SWAPCHAIN_EXTENSION_NAME 
};

// Constructor
Instance::Instance(){
	// Check if validation layers are available
	if (m_EnableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	// Vulkan application info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Application";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = "Elegy Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	// Get extensions to enable
	auto extensions = GetRequiredExtensions();

	// Instance creation info
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	
	// Enable validation layers if debug build, else 0 layers
	if (m_EnableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	// Create instance
	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan instance!");
	}

	// Set up debug messenger
	SetupDebugMessenger();
}

// Destructor
Instance::~Instance(){

	// Destroy debug messenger if validation layers are enabled
	if (m_EnableValidationLayers) {
		// Destroy debug messenger
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
	}

	// Destroy instance
	vkDestroyInstance(m_Instance, nullptr);

}

// Check if requested validation layers are supported
bool Instance::CheckValidationLayerSupport(){
	// Query supported layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Loop through all validation layers
	for (const char* layerName : m_ValidationLayers) {
		// Flag if layer was found
		bool layerFound = false;

		// Loop through available layers
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		// Return false if layer not found
		if (!layerFound) {
			return false;
		}
	}

	// Return true if all were found
	return true;
}

// Return required extensions
std::vector<const char*> Instance::GetRequiredExtensions(){
	// GLFW extensions
	uint32_t glfwExtensionCount;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Create vector from said extensions
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// Add debug utils extension if validation layers enabled
	if (m_EnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// Return vector of extensions
	return extensions;
}

// Set up debug messenger
void Instance::SetupDebugMessenger(){
	// Return if validation layers not enabled
	if (!m_EnableValidationLayers) return;

	// Debug messenger create info
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;

	// Create debug messenger
	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

// DEBUG LAYER FUNCTIONS
VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData){
	// Print error message
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	// Return false
	return VK_FALSE;
}

VkResult Instance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Instance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}