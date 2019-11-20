#pragma once

#include "../Graphics/Buffer.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Window.h"
#include "Test.h"

class TriangleTest : public Test {
public:
	TriangleTest();	// Constructor
	~TriangleTest();// Destructor
	
	// FUNCTIONS
	void Run();
};