#include "Vertex.h"

// Constructor
Vertex::Vertex(glm::vec3 position, glm::vec3 colour, glm::vec2 uv)
: m_Position(position), m_Colour(colour), m_UV(uv) {
	
}

// Destructor
Vertex::~Vertex() {

}

