#include "TriangleTest.h"

#include "../Graphics/Vertex.h"

#include <memory>

// Constructor
TriangleTest::TriangleTest(){
	m_Graphics = new Graphics();
	m_Window = m_Graphics->GetWindow();

	// Set window settings
	m_Window->SetResizable(true);

	// Create vertex buffer
	Vertex vert1({ 0.0f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0, 0 });
	Vertex vert2({ 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0, 0 });
	Vertex vert3({ -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0, 0 });
	std::vector<Vertex> vertices = { vert1, vert2, vert3 };
	m_Graphics->AddVertexBuffer(vertices);
}

// Destructor
TriangleTest::~TriangleTest(){
	
}

void TriangleTest::Run(){
	while (!m_Window->IsClosed()) {
		m_Window->Update();
		m_Graphics->Update();
	}
}
