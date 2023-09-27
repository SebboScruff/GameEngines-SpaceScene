#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Terrain.h"

namespace GE {
	class ModelRenderer {
	public:
		ModelRenderer(Model* m);
		virtual ~ModelRenderer();

		void init();

		void update();
		void draw(Camera* cam);
		void drawTerrain(Terrain* terrain);
		void destroy();

		// get position
		float getPosX() {
			return posX;
		}
		float getPosY() {
			return posY;
		}
		float getPosZ() {
			return posZ;
		}

		//get rotation
		float getRotX() {
			return rotX;
		}
		float getRotY() {
			return rotY;
		}
		float getRotZ() {
			return rotZ;
		}

		//get scale
		float getScaleX() {
			return scaleX;
		}
		float getScaleY() {
			return scaleY;
		}
		float getScaleZ() {
			return scaleZ;
		}

		// Mutator functions
		void setPos(float px, float py, float pz) {
			posX = px;
			posY = py;
			posZ = pz;
		}
		void setRot(float rx, float ry, float rz) {
			rotX = rx;
			rotY = ry;
			rotZ = rz;
		}
		void setScale(float sx, float sy, float sz) {
			scaleX = sx;
			scaleY = sy;
			scaleZ = sz;
		}

		void setMaterial(Texture* mat) {
			material = mat;
		}

	private:
		GLuint programID;

		GLint vertexPos3DLocation;

		GLint vertexUVLocation;

		GLuint vboModel;

		float posX, posY, posZ;
		float rotX, rotY, rotZ;
		float scaleX, scaleY, scaleZ;

		GLuint transformUniformID;
		GLuint viewUniformID;
		GLuint projectionUniformID;
		GLuint samplerID;

		Model* model;
		Texture* material;

	public:
		bool goUp = true; // bool for controlling vertical hover
	};
}