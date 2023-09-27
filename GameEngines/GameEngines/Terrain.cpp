#include "Terrain.h"

namespace GE {
	Terrain::Terrain(const std::string& heightMapFName, Texture* t) : texture(t) {
		imageSurface = IMG_Load(heightMapFName.c_str());
		width = imageSurface->w;
		height = imageSurface->h;
		channels = imageSurface->format->BytesPerPixel;
		imageData = (unsigned char*)imageSurface->pixels;

		std::vector<Vertex> vertices;
		// fill the vertex vector with data from the heightmap
		for (int row = 0; row < height; row++) {
			for (int column = 0; column < width; column++) {
				unsigned char redPixel = imageData[((row * width) + column) * channels];
				unsigned char greenPixel = imageData[((row * width) + column) * channels + 1];
				unsigned char bluePixel = imageData[((row * width) + column) * channels + 2];
				int meanPixelValue = (redPixel + greenPixel + bluePixel) / 3;

				x = (float)column / (float)(width - 1) * scaling;
				u = (float)column / (float)(width - 1);

				z = (float)row / (float)(height - 1) * scaling;
				v = (float)row / (float)(height - 1);

				y = meanPixelValue / 255.0f * heightScaling;

				vertices.emplace_back(x, y, z, u, v);
			}
		}

		std::vector<unsigned int> indices;
		unsigned int c, n;
		for (int row = 0; row < height - 1; row++) {
			for (int column = 0; column < width - 1; column++) {
				c = (row * width) + column;
				n = (row + 1) * width + column;

				indices.emplace_back(c);
				indices.emplace_back(n);
				indices.emplace_back(c+1);

				indices.emplace_back(c + 1);
				indices.emplace_back(n);
				indices.emplace_back(n + 1);
			}
		}

		indexCount = indices.size();
		vbo = createVBO(vertices);
		ibo = createIBO(indices);
		SDL_FreeSurface(imageSurface);
	}
}