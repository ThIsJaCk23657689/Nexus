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
			this->UpdateCameraVectors();
		}

		void SetPosition(glm::vec3 position) override {
			this->Set(position, this->WorldUp, this->Yaw, this->Pitch);
		}

		void SetWorldUp(glm::vec3 world_up) override {
			this->Set(this->Position, world_up, this->Yaw, this->Pitch);
		}

		void SetYaw(float yaw) override {
			this->Set(this->Position, this->WorldUp, yaw, this->Pitch);
		}

		void SetPitch(float pitch) override {
			this->Set(this->Position, this->WorldUp, this->Yaw, pitch);
		}
	};
}