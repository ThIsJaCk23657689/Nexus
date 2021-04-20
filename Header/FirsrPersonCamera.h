#pragma once

#include "Camera.h"

namespace Nexus {
	class FirstPersonCamera : public Camera {
	public:

		FirstPersonCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) {
			this->Set(position, up, yaw, pitch);
		}

		// Event
		void ProcessKeyboard(CameraMovement direction, float delta_time) override {
			float velocity = this->MovementSpeed * delta_time;
			if (direction == CAMERA_FORWARD) {
				this->Position += this->Front * velocity;
			}
			if (direction == CAMERA_BACKWARD) {
				this->Position -= this->Front * velocity;
			}
			if (direction == CAMERA_LEFT) {
				this->Position -= this->Right * velocity;
			}
			if (direction == CAMERA_RIGHT) {
				this->Position += this->Right * velocity;
			}

			if (this->Restrict) {
				this->Position = glm::clamp(this->Position, this->RestrictMin, this->RestrictMax);
			}
		}

		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override {
			xoffset *= this->MouseSensitivity;
			yoffset *= this->MouseSensitivity;

			this->Yaw += xoffset;
			this->Pitch += yoffset;

			if (constrainPitch) {
				if (this->Pitch > 89.0f) {
					this->Pitch = 89.0f;
				}
				if (this->Pitch < -89.0f) {
					this->Pitch = -89.0f;
				}
			}

			this->UpdateCameraVectors();
		}

		// Setter
		void Set(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
			this->Position = position;
			this->WorldUp = up;
			this->Yaw = yaw;
			this->Pitch = pitch;

			this->Restrict = false;
			this->RestrictMin = glm::vec3(-9.8f);
			this->RestrictMax = glm::vec3(9.8f);
			
			this->UpdateCameraVectors();
		}

		void SetPosition(glm::vec3 position) override {
			this->Position = position;
			this->UpdateCameraVectors();
		}

		void SetWorldUp(glm::vec3 world_up) override {
			this->WorldUp = world_up;
			this->UpdateCameraVectors();
		}

		void SetYaw(float yaw) override {
			this->Yaw = yaw;
			this->UpdateCameraVectors();
		}

		void SetPitch(float pitch) override {
			this->Pitch = pitch;
			this->UpdateCameraVectors();
		}
	};
}