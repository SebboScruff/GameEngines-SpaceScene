#pragma once
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string>

namespace GE {
	class Texture {
	public:
		Texture(std::string filename) {
			width = 0;
			height = 0;

			textureName = 0;

			loadTexture(filename);
		}

		Texture(GLuint textureName) {}

		~Texture(){}

		int getWidth() {
			return width;
		}
		int getHeight() {
			return height;
		}

		Uint32 getFormat() {
			return format;
		}

		GLuint getTextureName() {
			return textureName;
		}

	private:
		int width;
		int height;

		Uint32 format;

		GLuint textureName;

		void loadTexture(std::string filename);

	};
}