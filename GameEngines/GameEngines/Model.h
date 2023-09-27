#pragma once
#include "RenderUtils.h"

namespace GE {
	// this is commented out because the vertex struct is now in RenderUtils.h - keeping it around for old times' sake.
	/* Vertex {
		float x, y, z;
		float u, v;

		Vertex(float _x, float _y, float _z, float _u, float _v) {
			//initialise location:
			x = _x;
			y = _y;
			z = _z;

			//initialise colour:
			u = _u;
			v = _v;
		}
		Vertex() { //constructor with no parameters will default everything to 0
			x = y = z = 0.0f;
			u = v = 0.0f;
		}
	};*/

	class Model {
	public:
		Model() {
			vertices = nullptr;
			numVertices = 0;
		}
		~Model() {
			delete[] vertices;
		}

		bool loadFromFile(const char* filename);

		void* getVertices() {
			return (void*)vertices;
		}
		int getNumVertices() {
			return numVertices;
		}
	private:
		Vertex* vertices;
		int numVertices;
	};
}