#include "Shader.h"

#include <fstream>
#include <stdexcept>

// Constructor
Shader::Shader(Device* device, VkShaderStageFlagBits shaderStage, const std::string& shaderPath)
	: m_Device(device), m_ShaderStage(shaderStage) {
	// Get code
	auto shaderCode = ReadFile(shaderPath);

	// Create shader modules
	m_ShaderModule = CreateShaderModule(shaderCode);
}

// Destructor
Shader::~Shader() {
	// Destroy shader modules
	vkDestroyShaderModule(m_Device->GetDevice(), m_ShaderModule, nullptr);
}

// Read shader code into bytes
std::vector<char> Shader::ReadFile(const std::string& path){
	// Open file in binary
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	// Check if file was able to open
	if (!file.is_open()) {
		throw std::runtime_error("Unable to open file " + path);
	}

	// Get filesize and create buffer
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	// Go to beginning of file and read contents into buffer
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	// Close file and return buffer
	file.close();
	return buffer;
}

// Create shader module from code
VkShaderModule Shader::CreateShaderModule(const std::vector<char>& code){
	// Shader module creation info
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	// Create shader module
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_Device->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module!");
	}

	// Return shader module
	return shaderModule;
}

