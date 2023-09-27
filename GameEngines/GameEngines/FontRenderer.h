#pragma once

#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL.h>
#include <string>
#include <iostream>
#include "Texture.h"

namespace GE {
	class FontRenderer {
	public:
		FontRenderer() {}

		~FontRenderer() {}

		void init();

		Texture* renderText(std::string text, std::string fontname, int size);

		void destroy();

	private:

	};

	Texture* FontRenderer::renderText(std::string text, std::string fontname, int font_size) {
		// Load the text into an SDL Surface:
		TTF_Font* font = TTF_OpenFont(fontname.c_str(), font_size);
		if (font == nullptr) {
			std::cerr << "Error creating font " << text << "! " << TTF_GetError() << std::endl;
			return nullptr;
		}

		SDL_Color white = { 255, 255, 255 };

		SDL_Surface* surfaceImage = TTF_RenderText_Solid(font, text.c_str(), white);

		SDL_Surface* converted = SDL_ConvertSurfaceFormat(surfaceImage, SDL_PIXELFORMAT_RGBA32, 0); //convert to RGBA so it's supported OpenGL
		if (converted != nullptr) {
			SDL_FreeSurface(surfaceImage);
			surfaceImage = converted;
		}
		else {
			std::cerr << "Error Converting Font " << text << "! " << SDL_GetError() << std::endl;
			return nullptr;
		}

		// prepare the texture for openGL
		int width = surfaceImage->w;
		int height = surfaceImage->h;

		Uint32 format = GL_RGBA;

		if (SDL_MUSTLOCK(surfaceImage)) {
			SDL_LockSurface(surfaceImage);
		}
			
		//calc the number of bytes in a row
		int bytesPerRow = width * surfaceImage->format->BytesPerPixel;

		//create a temporary buffer for a row pixels
		char* temp = new char[bytesPerRow];

		//vertically flip the image - SDL UVs are top left, OpenGL UVs are bottom left

		for (int y = 0; y < height / 2; y++) {
			// copy bottom row into temp, top row to bottom row, temp to top row
			memcpy(temp, ((char*)surfaceImage->pixels) + ((height-y-1) * bytesPerRow), bytesPerRow);
			memcpy(((char*)surfaceImage->pixels) + ((height - y - 1) * bytesPerRow), (char*)surfaceImage->pixels + (y * bytesPerRow), bytesPerRow);
			memcpy((char*)surfaceImage->pixels + (y * bytesPerRow), temp, bytesPerRow);
		}

		delete[] temp;

		GLuint textureName;

		glGenTextures(1, &textureName);

		glBindTexture(GL_TEXTURE_2D, textureName);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surfaceImage->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(surfaceImage);
		TTF_CloseFont(font);

		return new Texture(textureName);
	}
}
