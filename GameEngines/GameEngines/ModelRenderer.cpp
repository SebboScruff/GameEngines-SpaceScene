#include "ModelRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

namespace GE {
	// this is commented out because the vertex struct is now in RenderUtils.h - keeping it around for old times' sake.
	/*struct Vertex {
		float x, y, z;
		float r, g, b, a;

		Vertex(float _x, float _y, float _z, float _r, float _g, float _b, float _a) {
			// location
			x = _x;
			y = _y;
			z = _z;

			//colour
			r = _r;
			b = _b;
			g = _g;
			a = _a;
		}
		Vertex() { //take in no parameters to default to 0 (on the origin and no colour)
			x = y = z = 0.0f;
			r = b = g = a = 0.0f;
		}
	};*/

	/*Vertex triangleVertices[] = {
		Vertex(-1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f)
	};*/

	ModelRenderer::ModelRenderer(Model* m) {
		posX = posY = posZ = 0.0f;
		rotX = rotY = rotZ = 0.0f;
		scaleX = scaleY = scaleZ = 1.0f;

		model = m;
	}

	ModelRenderer::~ModelRenderer() {

	}

	void displayShaderCompilerError(GLuint shaderID) {
		GLint msgLen = 0;

		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &msgLen);

		if (msgLen > 1) {
			GLchar* msg = new GLchar[msgLen + 1];

			glGetShaderInfoLog(shaderID, msgLen, NULL, msg);

			std::cerr << "Error compiling shaders: " << msg << std::endl;

			delete[] msg;
		}
	}

	void ModelRenderer::init() {
		// VERTEX SHADER
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* V_ShaderCode[] = {
			"#version 140 \n"
			"in vec3 vertexPos3D; \n"
			"in vec2 vUV; \n"
			"out vec2 uv; \n"
			"uniform mat4 transform;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main() {\n"
			"vec4 v = vec4(vertexPos3D.xyz, 1);\n"
			"v = projection * view * transform * v;\n"
			"gl_Position = v; \n"
			"uv = vUV; \n"
			"}\n"
		};

		glShaderSource(vertexShader, 1, V_ShaderCode, NULL);

		glCompileShader(vertexShader);

		GLint isShaderCompiledOK = GL_FALSE;

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile vertex shader." << std::endl;

			displayShaderCompilerError(vertexShader);
			return;
		}

		// FRAGMENT SHADER
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		const GLchar* F_ShaderCode[] = {
			"#version 140 \n"
			"in vec2 uv; \n"
			"uniform sampler2D sampler; \n"
			"out vec4 fragmentColour; \n"
			"void main() {\n"
			"fragmentColour = texture(sampler, uv).rgba; \n"
			"}\n"
		};

		glShaderSource(fragmentShader, 1, F_ShaderCode, NULL);

		glCompileShader(fragmentShader);

		isShaderCompiledOK = GL_FALSE;

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);
		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile fragment shader." << std::endl;

			displayShaderCompilerError(fragmentShader);
			return;
		}

		// PROGRAM OBJECT
		programID = glCreateProgram();

		glAttachShader(programID, vertexShader);
		glAttachShader(programID, fragmentShader);

		glLinkProgram(programID);

		// Check for linking errors:
		GLint isProgramLinked = GL_FALSE;
		glGetProgramiv(programID, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked != GL_TRUE) {
			std::cerr << "Failed to link program." << std::endl;
		}

		vertexPos3DLocation = glGetAttribLocation(programID, "vertexPos3D");
		if (vertexPos3DLocation == -1) {
			std::cerr << "Problem getting vertex3DPos" << std::endl;
		}

		vertexUVLocation = glGetAttribLocation(programID, "vUV");
		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl;
		}

		transformUniformID = glGetUniformLocation(programID, "transform");
		viewUniformID = glGetUniformLocation(programID, "view");
		projectionUniformID = glGetUniformLocation(programID, "projection");
		samplerID = glGetUniformLocation(programID, "sampler");

		glGenBuffers(1, &vboModel);
		glBindBuffer(GL_ARRAY_BUFFER, vboModel);

		glBufferData(GL_ARRAY_BUFFER, model->getNumVertices() * sizeof(Vertex), model->getVertices(), GL_STATIC_DRAW);
	}

	void ModelRenderer::update() {

	}

	void ModelRenderer::draw(Camera* cam) {
		glEnable(GL_CULL_FACE);

		glUseProgram(programID);

		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		// declare and calculate the transformation matrix for the triangle
		glm::mat4 transformationMat = glm::mat4(1.0f);

		transformationMat = glm::translate(transformationMat, glm::vec3(posX, posY, posZ));
		transformationMat = glm::rotate(transformationMat, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
		transformationMat = glm::scale(transformationMat, glm::vec3(scaleX, scaleY, scaleZ));

		glUniformMatrix4fv(transformUniformID, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformID, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformID, 1, GL_FALSE, glm::value_ptr(projectionMat));

		glBindBuffer(GL_ARRAY_BUFFER, vboModel);

		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

		glVertexAttribPointer(vertexUVLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glEnableVertexAttribArray(vertexUVLocation);

		glEnableVertexAttribArray(vertexPos3DLocation);

		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerID, 0);
		glBindTexture(GL_TEXTURE_2D, material->getTextureName());

		// Draw the Model
		glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices());

		glDisableVertexAttribArray(vertexPos3DLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		glDisable(GL_CULL_FACE);

		glUseProgram(0);
	}
	
	void ModelRenderer::drawTerrain(Terrain* terrain) {
		glUseProgram(programID);

		// Vertex Positions:
		glEnableVertexAttribArray(vertexPos3DLocation);
		glBindBuffer(GL_ARRAY_BUFFER, terrain->getVBO());
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

		glEnableVertexAttribArray(vertexUVLocation);
		glVertexAttribPointer(vertexUVLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		// Texture:
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerID, 0);
		glBindTexture(GL_TEXTURE_2D, terrain->getTexture()->getTextureName());

		// Indices:
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->getIBO());
		glDrawElements(GL_TRIANGLES, terrain->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindTexture(GL_TEXTURE_2D, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

		glUseProgram(NULL);
	}

	void ModelRenderer::destroy() {
		glDeleteProgram(programID);

		glDeleteBuffers(1, &vboModel);
	}
}