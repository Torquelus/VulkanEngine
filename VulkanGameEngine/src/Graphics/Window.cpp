#include "Window.h"

#include <stdexcept>

// Constructor
Window::Window(std::string title, int width, int height)
: m_Title(title), m_Width(width), m_Height(height) {
	
	// Initialise GLFW
	if (glfwInit() != GLFW_TRUE) {
		throw std::runtime_error("Failed to initialise GLFW!");
	}

	// Set window hidden for creation
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	// Disable context creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// No stereo view
	glfwWindowHint(GLFW_STEREO, GLFW_FALSE);
	
	// Create GLFW window
	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

	// Check window was created
	if (!m_Window) {
		throw std::runtime_error("Failed to creat GLFW window!");
	}

	// Make window borderless
	glfwSetWindowAttrib(m_Window, GLFW_DECORATED, !m_Borderless);
	// Make window resizable
	glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, m_Resizable);
	// Make window floating
	glfwSetWindowAttrib(m_Window, GLFW_FLOATING, m_Floating);

	// Shows the glfw window.
	glfwShowWindow(m_Window);

	// Set framebuffer resize callback function
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);

}

// Destructor
Window::~Window(){

	// Destroy window
	glfwDestroyWindow(m_Window);

	// Terminate GLFW
	glfwTerminate();

}

VkResult Window::CreateSurface(const VkInstance& instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface){
	auto result = glfwCreateWindowSurface(instance, m_Window, allocator, surface);
	return result;
}

// SETTERS

void Window::SetTitle(std::string title){
	m_Title = title;
	glfwSetWindowTitle(m_Window, m_Title.c_str());
}


void Window::SetSize(int width, int height){
	m_Width = width;
	m_Height = height;
	glfwSetWindowSize(m_Window, m_Width, m_Height);
}

void Window::SetBorderless(bool borderless){
	m_Borderless = borderless;
	glfwSetWindowAttrib(m_Window, GLFW_DECORATED, !m_Borderless);
}

void Window::SetResizable(bool resizable){
	m_Resizable = resizable;
	glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, m_Resizable);
}

void Window::SetFloating(bool floating){
	m_Floating = floating;
	glfwSetWindowAttrib(m_Window, GLFW_FLOATING, m_Floating);
}

void Window::UpdateSize(){
	// Initialise to zeroes
	int width = 0, height = 0;

	// Wait until nonzero width and height
	while (width = 0 || height == 0) {
		glfwGetFramebufferSize(m_Window, &width, &height);
		glfwWaitEvents();
	}

	// Update width and height
	m_Width = width;
	m_Height = height;
}

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height){
	auto windowObject = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	windowObject->SetFramebufferResized(true);
}
