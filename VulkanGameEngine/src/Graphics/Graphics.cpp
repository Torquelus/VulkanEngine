#include "Graphics.h"
#include "Window.h"

#include <stdexcept>

// Static members
//std::unique_ptr<Graphics> Graphics::m_Graphics = std::make_unique<Graphics>();

// Constructor
Graphics::Graphics()
	: m_Window(std::make_unique<Window>("Vulkan Window", 800, 600)),
	m_Instance(std::make_unique<Instance>()),
	m_PhysicalDevice(std::make_unique<PhysicalDevice>(m_Instance.get())),
	m_Surface(std::make_unique<Surface>(m_Instance.get(), m_PhysicalDevice.get(), m_Window.get())),
	m_Device(std::make_unique<Device>(m_Instance.get(), m_PhysicalDevice.get(), m_Surface.get())),
	m_CommandPool(std::make_unique<CommandPool>(m_Device.get(), m_PhysicalDevice.get())){
}

// Destructor
Graphics::~Graphics() {
	// Wait for device to idle
	vkDeviceWaitIdle(m_Device->GetDevice());

	// Destroy semaphores and fences
	for (size_t i = 0; i < m_ImageAvailableSemaphores.size(); i++) {
		vkDestroySemaphore(m_Device->GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device->GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_Device->GetDevice(), m_FlightFences[i], nullptr);
	}

	// Delete command buffers
	for (auto& commandBuffer : m_CommandBuffers) {
		delete(commandBuffer);
	}
}

// Graphics update function
void Graphics::Update(){
	// Wait for fences
	vkWaitForFences(m_Device->GetDevice(), 1, &m_FlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
	
	// Acquire next image in swapchain and return result
	auto acquireResult = m_Swapchain->AcquireNextImage(m_ImageAvailableSemaphores[m_CurrentFrame]);

	// Check if swapchain needs to be recreated
	if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapchain();
		return;
	}
	else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	// Get active image index
	auto imageIndex = m_Swapchain->GetActiveImageIndex();

	// Check if fence in use
	if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		// Wait for fence to finish
		vkWaitForFences(m_Device->GetDevice(), 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	// Update images in flight
	m_ImagesInFlight[imageIndex] = m_FlightFences[m_CurrentFrame];

	// Submit to graphics queue
	m_CommandBuffers[imageIndex]->Submit(m_ImageAvailableSemaphores[m_CurrentFrame], m_RenderFinishedSemaphores[m_CurrentFrame], m_FlightFences[m_CurrentFrame]);

	// Present
	auto presentResult = m_Swapchain->QueuePresent(m_Device->GetPresentQueue(), m_RenderFinishedSemaphores[m_CurrentFrame]);

	// Check if swapchain needs to be recreated
	if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || m_Window->GetFramebufferResized()) {
		RecreateSwapchain();
		return;
	}
	else if (presentResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image!");
	}

	// Update current frame
	m_CurrentFrame = (m_CurrentFrame + 1) % m_Swapchain->GetImageCount();
}

// Add vertex buffer to graphics
void Graphics::AddVertexBuffer(std::vector<Vertex> vertices){
	// Add buffer
	m_VertexBuffers.emplace_back(new Buffer(m_Device.get(), m_PhysicalDevice.get(), sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data()));

	// Recreate swapchain
	RecreateSwapchain();
}

void Graphics::CreateSyncObjects(){
	// Exit if already created
	if (m_FlightFences.size() != 0) {
		return;
	}

	// Resize vectors
	m_ImageAvailableSemaphores.resize(m_Swapchain->GetImageCount());
	m_RenderFinishedSemaphores.resize(m_Swapchain->GetImageCount());
	m_FlightFences.resize(m_Swapchain->GetImageCount());
	m_ImagesInFlight.resize(m_Swapchain->GetImageCount(), VK_NULL_HANDLE);

	// Semaphore creation info
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// Fence creation info
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	// Create semaphores and fences
	for (size_t i = 0; i < m_ImageAvailableSemaphores.size(); i++) {
		// Create image available semaphore
		if (vkCreateSemaphore(m_Device->GetDevice(), &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("Unable to create image available semaphore!");
		}
		// Create render finished semaphore
		if (vkCreateSemaphore(m_Device->GetDevice(), &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("Unable to create render finished semaphore!");
		}
		// Create in flight fences
		if (vkCreateFence(m_Device->GetDevice(), &fenceCreateInfo, nullptr, &m_FlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("Unable to create in-flight fence!");
		}
	}
}

// Fill in vector of command buffers
void Graphics::RecreateCommandBuffers(){
	// Resize command buffers vector
	m_CommandBuffers.resize(m_Swapchain->GetImageCount());

	// Create command buffer to fill vector
	for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
		// Create command buffer and begin
		m_CommandBuffers[i] = new CommandBuffer(m_Device.get(), m_CommandPool.get());
		m_CommandBuffers[i]->Begin(0);

		// Begin render pass
		m_RenderPass->Begin(m_CommandBuffers[i]->GetCommandBuffer(), m_SwapchainFramebuffers->GetFramebuffers()[i]);

		// Bind graphics pipeline
		m_GraphicsPipeline->Bind(m_CommandBuffers[i]->GetCommandBuffer());

		// Vulkan draw command for all buffers
		for (auto& buffer : m_VertexBuffers) {
			// Bind buffer
			buffer->Bind(m_CommandBuffers[i]->GetCommandBuffer());
			vkCmdDraw(m_CommandBuffers[i]->GetCommandBuffer(), 3, 1, 0, 0);
		}

		// End render pass
		m_RenderPass->End(m_CommandBuffers[i]->GetCommandBuffer());

		// End command buffer recording
		m_CommandBuffers[i]->End();
	}
}

// Recreate swapchain for resized window
void Graphics::RecreateSwapchain(){
	// Wait for device to idle
	vkDeviceWaitIdle(m_Device->GetDevice());
	m_CurrentFrame = 0;

	// Create new swapchain
	m_Swapchain = std::make_unique<Swapchain>(m_Device.get(), m_PhysicalDevice.get(), m_Surface.get(), m_Window.get());
	m_RenderPass = std::make_unique<RenderPass>(m_Device.get(), m_Swapchain.get());
	m_GraphicsPipeline = std::make_unique<GraphicsPipeline>(m_Device.get(), m_Swapchain.get(), m_RenderPass.get());
	m_SwapchainFramebuffers = std::make_unique<Framebuffers>(m_Device.get(), m_RenderPass.get(), m_Swapchain.get());

	// Recreate command buffers
	RecreateCommandBuffers();
	CreateSyncObjects();

	// Set bool
	m_Window->SetFramebufferResized(false);
}
