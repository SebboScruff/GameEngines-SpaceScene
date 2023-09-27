#include "Texture.h"

namespace GE {
	void Texture::loadTexture(std::string filename) {
		SDL_Surface* surfaceImage = IMG_Load(filename.c_str());
		if (surfaceImage == nullptr) { return; }

		width = surfaceImage->w;
		height = surfaceImage->h;

		format = surfaceImage->format->format;

		switch (format) {
		case SDL_PIXELFORMAT_RGBA32:
			format = GL_RGBA;
			break;
		case SDL_PIXELFORMAT_RGB24:
			format = GL_RGB;
			break;
		default:
			format = GL_RGB;
			break;
		}

		// create a texture name
		glGenTextures(1, &textureName);
		//select a created texture for subsequent operations
		glBindTexture(GL_TEXTURE_2D, textureName);
		//copt the pixel data from the SDL surface to the openGL texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surfaceImage->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(surfaceImage);

		return;
	}
}