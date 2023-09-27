#pragma once
#include <SDL_image.h>
#include <string>
#include <vector>
#include <iostream>
#include "RenderUtils.h"
#include "Texture.h"

namespace GE {
	class Terrain {
	public:
		Terrain(const std::string& heightMapFName, Texture* t);
		~Terrain() {}

		unsigned int getVBO() {
			return vbo;
		}
		unsigned int getIBO() {
			return ibo;
		}
		unsigned int getIndexCount() {
			return indexCount;
		}

		Texture* getTexture() {
			return texture;
		}

	private:
		float x, y, z, u, v;
		int height, width, channels;
		float scaling = 100.0f, heightScaling = 3.0f;
		SDL_Surface* imageSurface;
		unsigned char* imageData;
		unsigned int vbo, ibo, indexCount;

		Texture* texture;
	};
}