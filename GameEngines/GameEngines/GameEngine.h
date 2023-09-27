#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "ModelRenderer.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"
#include "Terrain.h"

namespace GE {
	class GameEngine {
	public:
		GameEngine();
		virtual ~GameEngine();

		bool init(bool vsync = false);
		bool keepRunning();
		void processInput();
		void update();
		void draw();
		void shutdown();

		void setWindowTitle(const char*);

	private:
		SDL_Window* window;

		SDL_GLContext glContext;

		Camera* cam;
		
		// Model Renderer 1: a rock and the terrain
		ModelRenderer* mr1;
		Model* m1;
		Texture* mat1;
		Terrain* terrain;
		Texture* terrainTexture;

		// Just a temporary fix to get lots of objects in the scene; TODO more memory-efficient way of doing this
		//Model Renderers 2-5: more rocks
		ModelRenderer* mr2;
		Model* m2;
		Texture* mat2;

		ModelRenderer* mr3;
		Model* m3;
		Texture* mat3;

		ModelRenderer* mr4;
		Model* m4;
		Texture* mat4;

		ModelRenderer* mr5;
		Model* m5;
		Texture* mat5;

		SkyboxRenderer* skybox;

		
	};

	void displayInfoMessage(const char*);
}