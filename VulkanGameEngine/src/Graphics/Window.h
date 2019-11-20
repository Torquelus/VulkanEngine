#pragma once

#define GLFW_INCLUDE_VULKAN

#include <memory>
#include <GLFW/glfw3.h>
#include <string>

class Window {

public:
	Window(std::string title, int width, int height);	// Constructor
	~Window();	// Destructor

	VkResult CreateSurface(const VkInstance& instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);
	void Update() { glfwPollEvents(); }							// Poll for GLFW events
	bool IsClosed(){ return glfwWindowShouldClose(m_Window); }	// Return true if window is being closed

	// GETTERS
	static Window* Get() { return m_WindowInstance.get(); }
	const std::string GetTitle() const { return m_Title; }
	int GetWidth() {
		UpdateSize();
		return m_Width; 
	}
	int GetHeight() { 
		UpdateSize();
		return m_Height; 
	}
	const bool GetBorderless() const { return m_Borderless; }
	const bool GetResizable() const { return m_Resizable; }
	const bool GetFloating() const { return m_Floating; }
	const bool GetFramebufferResized() const { return m_FramebufferResized; }

	// SETTERS
	void SetTitle(std::string title);
	void SetSize(int width, int height);
	void SetBorderless(bool borderless);
	void SetResizable(bool resizable);
	void SetFloating(bool floating);
	void SetFramebufferResized(bool framebufferResized) { m_FramebufferResized = framebufferResized; }
private:
	static std::unique_ptr<Window>(m_WindowInstance);
	GLFWwindow* m_Window;	// GLFW Window

	// Window details
	int m_Width;
	int m_Height;
	std::string m_Title;

	// Window bools
	bool m_Borderless = false;
	bool m_Resizable = false;
	bool m_Floating = false;
	bool m_FramebufferResized = false;

	// FUNCTIONS
	void UpdateSize();
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

};