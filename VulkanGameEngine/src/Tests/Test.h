#pragma once

#include "../Graphics/Graphics.h"
#include "../Graphics/Window.h"

class Test {
public:
	virtual void Run() = 0;	// Run application
protected:
	// Variables
	Window* m_Window;
	Graphics* m_Graphics;
};