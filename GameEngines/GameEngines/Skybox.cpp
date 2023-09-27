#include "Skybox.h"
#include "ShaderUtils.h"
#include <SDL_image.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace GE {
	struct CubeVertex {
		float x, y, z;

		CubeVertex() {
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}
		CubeVertex(float _x, float _y, float _z) {
			x = _x;
			y = _y;
			z = _z;
		}
	};

	// Define the cube as an array of vertices
	const float SIDE = 1.0f;

	CubeVertex cube[] = {
		// Front face
		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),

		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),

		// Back face
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(SIDE, -SIDE, SIDE),

		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, SIDE, SIDE),

		// Left face
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, -SIDE),

		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE),

		// Right face
		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE,  SIDE),

		// Top face
		CubeVertex(-SIDE, SIDE,  SIDE),
		CubeVertex(SIDE, SIDE,  SIDE),
		CubeVertex(SIDE, SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE,  SIDE),

		// Bottom face
		CubeVertex(-SIDE, -SIDE,  SIDE),
		CubeVertex(SIDE, -SIDE,  SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),

		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE),
	};

	void SkyboxRenderer::createCubemap(std::vector<std::string> filenames) {
		//create a name for the cubemap
		glGenTextures(1, &skyboxCubeMapName);

		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		for (int faceNum = 0; faceNum < 6; faceNum++) {
			SDL_Surface* surfaceImage = IMG_Load(filenames[faceNum].c_str());
			if (surfaceImage == nullptr) { return; }

			GLenum format = surfaceImage->format->format;

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

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceNum, 0, format, surfaceImage->w, surfaceImage->h, 0, format, GL_UNSIGNED_BYTE, surfaceImage->pixels);

			SDL_FreeSurface(surfaceImage);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Clamp the edges of the skybox to remove seams
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void SkyboxRenderer::createCubeVBO() {
		// create the vertex buffer object
		glGenBuffers(1, &vboSkybox);
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		// transfer vertices to graphics memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);
	}

	void SkyboxRenderer::createSkyboxProgram() {
		const GLchar* V_ShaderCode[] = {
			"#version 140\n"
			"in vec3 vertexPos3D;\n"
			"out vec3 texCoord;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main() {\n"
			"vec4 v = vec4(vertexPos3D, 1);\n"
			"v = projection * view * v;\n"
			"gl_Position = v;\n"
			"texCoord = vertexPos3D;\n"
			"}\n"
		};

		const GLchar* F_ShaderCode[] = {
			"#version 140\n"
			"in vec3 texCoord;\n"
			"uniform samplerCube sampler;\n"
			"out vec4 fragmentColour;\n"
			"void main() {\n"
			"fragmentColour = vec4(texture(sampler, texCoord).rgb, 1.0f);\n"
			"}\n"
		};

		bool result = compileProgram(V_ShaderCode, F_ShaderCode, &skyboxProgramID);
		if (!result) {
			std::cerr << "Failed to create skybox program - check console for errors" << std::endl;
			return;
		}

		vertexLocation = glGetAttribLocation(skyboxProgramID, "vertexPos3D");
		if (vertexLocation == -1) { std::cerr << "Problem getting vertex3DPos" << std::endl; }

		viewUniformID = glGetUniformLocation(skyboxProgramID, "view");
		projectionUniformID = glGetUniformLocation(skyboxProgramID, "projection");
		samplerID = glGetUniformLocation(skyboxProgramID, "sampler");
	}

	void SkyboxRenderer::draw(Camera* cam) {
		bool isDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);

		glm::mat4 camView = cam->getViewMatrix();
		glm::mat4 projection = cam->getProjectionMatrix();
		// set viewcam translations to 0 to make sure that the skybox is centered at the origin
		camView[3][0] = 0.0f;
		camView[3][1] = 0.0f;
		camView[3][2] = 0.0f;

		// get the program into the rendering context
		glUseProgram(skyboxProgramID);

		// set the uniforms in the shader
		glUniformMatrix4fv(viewUniformID, 1, GL_FALSE, glm::value_ptr(camView));
		glUniformMatrix4fv(projectionUniformID, 1, GL_FALSE, glm::value_ptr(projection));

		glEnableVertexAttribArray(vertexLocation);

		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, x));

		// select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerID, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		// draw the model
		glDrawArrays(GL_TRIANGLES, 0, sizeof(cube) / sizeof(CubeVertex));

		glDisableVertexAttribArray(vertexLocation);

		glUseProgram(0);

		if (isDepthTestEnabled) { glEnable(GL_DEPTH_TEST); }
	}

	void SkyboxRenderer::destroy() {
		glDeleteProgram(skyboxProgramID);
		glDeleteBuffers(1, &vboSkybox);
		glDeleteTextures(1, &skyboxCubeMapName);
	}
}