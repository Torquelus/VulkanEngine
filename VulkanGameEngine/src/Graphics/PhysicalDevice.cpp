#include "PhysicalDevice.h"

#include <map>
#include <set>
#include <stdexcept>

// Constructor
PhysicalDevice::PhysicalDevice(const Instance* instance)
: m_Instance(instance) {
	PickPhysicalDevice();
}

// Destructor
PhysicalDevice::~PhysicalDevice(){
	
}

// Function that picks physical device
void PhysicalDevice::PickPhysicalDevice(){

	// Query count of physical devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance->GetInstance(), &deviceCount, nullptr);

	// Throw error if no devices found
	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU with Vulkan support!");
	}

	// Query physical devices
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance->GetInstance(), &deviceCount, devices.data());

	// Unordered map to sort candidate devices
	std::multimap<int, VkPhysicalDevice> candidates;

	// Loop through all found devices and insert to candidates with score
	for (const auto& device : devices) {
		// Get device score
		int score = RateDevice(device);

		// Insert to map
		candidates.insert(std::make_pair(score, device));
	}

	// Check if best candidate is suitable
	if (candidates.rbegin()->first > 0) {
		m_PhysicalDevice = candidates.rbegin()->second;
	}
	else {
		throw std::runtime_error("Failed to find suitable GPU!");
	}

	// Query physical device properties and features
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);
}

// Function that rates all physical devices and returns score
int PhysicalDevice::RateDevice(VkPhysicalDevice device){
	// Query device features and properties
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	// Initialise score at 0
	int score = 0;

	// Add 1000 score if discrete
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Add maximum possible size of textures to score
	score += deviceProperties.limits.maxImageDimension2D;

	// Return 0 if no geometry shader possible
	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	// Return 0 if extensions not supported
	if (!CheckDeviceExtensionSupport(device)) {
		return 0;
	}

	// Return device score
	return score;
}

// Check extension support on physical device
bool PhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device){
	// Enumerate extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// Set of required extensions
	std::set<std::string> requiredExtensions(Instance::m_DeviceExtensions.begin(), Instance::m_DeviceExtensions.end());

	// Loop through available extensions
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	// Return true if all required extensions were found
	return requiredExtensions.empty();
}
