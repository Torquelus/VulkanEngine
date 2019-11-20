#pragma once

#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

class Vertex {
public:
	Vertex(glm::vec3 position, glm::vec3 colour, glm::vec2 uv);	// Constructor
	~Vertex();	// Destructor

	// Operator overloads
	bool operator==(const Vertex& other) const {
		return m_Position == other.GetPosition() && m_Colour == other.GetColour() && m_UV == other.GetUV();
	}

	bool operator!=(const Vertex& other) const {
		return !operator==(other);
	}

	// GETTERS
	const glm::vec3 GetPosition() const { return m_Position; }
	const glm::vec3 GetColour() const { return m_Colour; }
	const glm::vec2 GetUV() const { return m_UV; }

	// STATIC GETTERS
	static VkVertexInputBindingDescription GetBindingDescription(){ 
		VkVertexInputBindingDescription bindingDescription = {};

		// Create binding description
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		
		// Fill in vertex attribute description for position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, m_Position);

		// Fill in vertex attribute description for colour
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, m_Colour);

		return attributeDescriptions;
	}
private:
	// VARIABLES
	glm::vec3 m_Position;
	glm::vec3 m_Colour;
	glm::vec2 m_UV;

};