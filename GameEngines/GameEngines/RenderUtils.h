#pragma once

#include <vector>
#include <gl/glew.h>

namespace GE
{
	struct Vertex {
		Vertex(float xPos, float yPos, float zPos, float uCoord, float vCoord) {
			x = xPos;
			y = yPos;
			z = zPos;
			u = uCoord;
			v = vCoord;

		}
		Vertex() { //constructor with no parameters will default everything to 0
			x = y = z = 0.0f;
			u = v = 0.0f;
		}
		float x, y, z, u, v;
	};

	unsigned int createVBO(const std::vector<Vertex>& vertices);
	unsigned int createIBO(const std::vector<unsigned int>& indices);
};