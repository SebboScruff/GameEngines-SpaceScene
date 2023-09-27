#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GE {
	class Camera {
	public:
		Camera(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 camUpDir, float fov, float ar, float near, float far) {
			pos = camPos;
			target = camTarget;
			up = camUpDir;

			fovy = fov;
			aspectR = ar;
			nearClip = near;
			farClip = far;

			updateCamMatrices();

			//SDL_GetMouseState(&oldMouseX, &oldMouseY);
		}

		~Camera(){}

		// Accessor Methods
		float getPosX() {
			return pos.x;
		}
		float getPosY() {
			return pos.y;
		}
		float getPosZ() {
			return pos.z;
		}
		glm::vec3 getPos() {
			return pos;
		}

		glm::vec3 getTarget() {
			return target;
		}
		glm::vec3 getUpDir() {
			return up;
		}

		float getPitch() {
			return pitch;
		}
		float getYaw() {
			return yaw;
		}

		float getOldMouseX() {
			return oldMouseX;
		}
		float getOldMouseY() {
			return oldMouseY;
		}

		glm::mat4 getViewMatrix() {
			return viewMat;
		}
		glm::mat4 getProjectionMatrix() {
			return projectionMat;
		}

		// Mutator Methods
		void SetPosX(float newX) {
			pos = glm::vec3(newX, pos.y, pos.z);
			updateCamMatrices();
		}
		void SetPosY(float newY) {
			pos = glm::vec3(pos.x, newY, pos.z);
			updateCamMatrices();
		}
		void SetPosZ(float newZ) {
			pos = glm::vec3(pos.x, pos.y, newZ);
			updateCamMatrices();
		}
		void SetPos(float newX, float newY, float newZ) {
			pos = glm::vec3(newX, newY, newZ);
			updateCamMatrices();
		}
		void SetPos(glm::vec3 newPos) {
			pos = newPos;
			updateCamMatrices();
		}
		
		void setTarget(glm::vec3 newTarget) {
			target = newTarget;
			updateCamMatrices();
		}
		void setUpDir(glm::vec3 newUpDir) {
			up = newUpDir;
			updateCamMatrices();
		}

		void setPitch(float newPitch) {
			pitch = newPitch;
			// Clamp the pitch values so that the camera can't just spin around a full circle
			if (pitch > 70.0f) { pitch = 70.0f; }
			if (pitch < -70.0f) { pitch = -70.0f; }
		}
		void setYaw(float newYaw) {
			yaw = newYaw;
		}

		void setOldMouseX(float newOldMouseX) {
			oldMouseX = newOldMouseX;
		}
		void setOldMouseY(float newOldMouseY) {
			oldMouseY = newOldMouseY;
		}

		void setFov(float newFov) {
			fovy = newFov;
			updateCamMatrices();
		}
		void setAspectRatio(float newAR) {
			aspectR = newAR;
			updateCamMatrices();
		}
		void setNearClip(float newNearClip) {
			nearClip = newNearClip;
			updateCamMatrices();
		}
		void setFarClip(float newFarClip) {
			farClip = newFarClip;
			updateCamMatrices();
		}

		void updateCamMatrices() {
			viewMat = glm::lookAt(pos, pos + target, up);
			projectionMat = glm::perspective(glm::radians(fovy), aspectR, nearClip, farClip);
		}

	private:
		
		glm::vec3 pos, target, up;

		float fovy, aspectR, nearClip, farClip;

		float pitch = 0.0f;
		float yaw = -90.0f;

		int oldMouseX, oldMouseY;

		glm::mat4 viewMat, projectionMat;
	};
}