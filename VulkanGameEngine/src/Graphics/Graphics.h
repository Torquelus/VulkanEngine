#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "Buffer.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Framebuffers.h"
#include "GraphicsPipeline.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "RenderPass.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Vertex.h"
#include "Window.h"

class Graphics {
public:
	Graphics();		// Constructor
	~Graphics();	// Destructor

	// FUNCTIONS
	void Update();	// Graphics update function
	void AddVertexBuffer(std::vector<Vertex> vertices);		// Add buffer to buffer vector

	// GETTERS
	//static Graphics* Get() { return m_Graphics.get(); }
	Window* GetWindow() { return m_Window.get(); }
	Surface* GetSurface() { return m_Surface.get(); }
	Instance* GetInstance() { return m_Instance.get(); }
	Device* GetDevice() { return m_Device.get(); }
	PhysicalDevice* GetPhysicalDevice() { return m_PhysicalDevice.get(); }
private:
	// VARIABLES
	//static std::unique_ptr<Graphics> m_Graphics;	// Graphics static object
	std::unique_ptr<Window> m_Window;				// Window static object
	std::unique_ptr<Instance> m_Instance;			// Vulkan instance
	std::unique_ptr<PhysicalDevice> m_PhysicalDevice;// Vulkan physical device
	std::unique_ptr<Surface> m_Surface;				// Vulkan surface
	std::unique_ptr<Device> m_Device;				// Vulkan logical device
	std::unique_ptr<Swapchain> m_Swapchain;			// Vulkan swapchain
	std::unique_ptr<RenderPass> m_RenderPass;				// Vulkan render pass
	std::unique_ptr<GraphicsPipeline> m_GraphicsPipeline;	// Vulkan graphics pipeline
	std::unique_ptr<Framebuffers> m_SwapchainFramebuffers;	// Vulkan swapchain framebuffers
	std::unique_ptr<CommandPool> m_CommandPool;				// Vulkan command pool

	size_t m_CurrentFrame = 0;						// Current frame
	std::vector<CommandBuffer*> m_CommandBuffers;	// Vector of command buffers
	std::vector<VkSemaphore> m_ImageAvailableSemaphores;	// Vector of image available semaphores
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;	// Vector of render finished semaphores
	std::vector<VkFence> m_FlightFences;					// Vector of in flight fences
	std::vector<VkFence> m_ImagesInFlight;					// Vector of fences for images in flight

	std::vector<Buffer*> m_VertexBuffers = {};

	// FUNCTIONS
	void CreateSyncObjects();		// Create semaphores and fences
	void RecreateCommandBuffers();	// Fill in vector of command buffers
	void RecreateSwapchain();		// Recreate swapchain for resized window
};