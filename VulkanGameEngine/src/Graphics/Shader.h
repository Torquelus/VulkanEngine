#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "Device.h"

class Shader {
public:
	Shader(Device* device, VkShaderStageFlagBits shaderStage, const std::string& shaderPath);	// Constructor
	~Shader();	// Destructor

	// GETTERS
	VkShaderModule GetShaderModule() { return m_ShaderModule; }
	VkShaderStageFlagBits GetShaderStage() { return m_ShaderStage; }
private:
	// VARIABLES
	Device* m_Device;

	VkShaderModule m_ShaderModule;
	VkShaderStageFlagBits m_ShaderStage;

	// FUNCTIONS
	static std::vector<char> ReadFile(const std::string& path);	// Read shader code into bytes
	VkShaderModule CreateShaderModule(const std::vector<char>& code);	// Create shader module from code
};