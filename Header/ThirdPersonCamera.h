#pragma once

#include "Camera.h"

namespace Nexus {
	class ThirdPersonCamera : public Camera {
	public:
		
		ThirdPersonCamera(glm::vec3 position = glm::vec3(0.0f, 5.0f, 2.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) {
			this->Set(position, target, up, yaw, pitch);
		}

		// Event
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override {
			xoffset *= this->MouseSensitivity;
			yoffset *= this->MouseSensitivity;

			Yaw += xoffset;
			if (this->Yaw > 360) {
				this->Yaw -= 360;
			}
			if (this->Yaw < -360) {
				this->Yaw += 360;
			}
			
			Pitch += yoffset;
			if (this->Pitch > 360) {
				this->Pitch -= 360;
			}
			if (this->Pitch < -360) {
				this->Pitch += 360;
			}

			if (constrainPitch) {
				if (this->Pitch > 89.0f) {
					this->Pitch = 89.0f;
				}
				if (this->Pitch < -89.0f) {
					this->Pitch = -89.0f;
				}
			}

			// 記得，因為攝影機與物體的距離是固定的，所以當你的視角發現變化時，你的位置也會跟著不同。
			this->UpdateCameraPosition();
		}
		
		// Getter
		glm::mat4 GetViewMatrix() override {
			return this->CalcLookAtMatrix(this->Position, this->Target, this->WorldUp);
		}
		

		float GetDistance() const { return this->Distance; }
		float* GetDistanceHepler() { return &this->Distance; }
		
		// Setter
		void Set(glm::vec3 position, glm::vec3 target, glm::vec3 up, float yaw, float pitch) {
			this->Position = position;
			this->Target = target;
			this->Distance = glm::length(this->Position - this->Target);
			this->WorldUp = up;
			this->Yaw = yaw;
			this->Pitch = pitch;

			this->Restrict = false;
			this->RestrictMin = glm::vec3(-9.8f);
			this->RestrictMax = glm::vec3(9.8f);
			
			this->UpdateCameraPosition();
		}

		void SetPosition(glm::vec3 position) override {
			this->Position = position;
			this->UpdateCameraPosition();
		}

		void SetTarget(glm::vec3 target) {
			this->Target = target;
			this->UpdateCameraPosition();
		}

		void SetWorldUp(glm::vec3 world_up) override {
			this->WorldUp = world_up;
			this->UpdateCameraPosition();
		}

		void SetYaw(float yaw) override {
			this->Yaw = yaw;
			this->UpdateCameraPosition();
		}

		void SetPitch(float pitch) override {
			this->Pitch = pitch;
			this->UpdateCameraPosition();
		}

		// 縮短或拉長攝影機與物體之間的距離
		void AdjustDistance(float yoffset, float min = 100.0f, float max = 1000.0f, float stride = 10.0f) {
			if (this->Distance >= min && this->Distance <= max) {
				this->Distance -= (float)yoffset * stride;
			}
			if (this->Distance < min) {
				this->Distance = min;
			}
			if (this->Distance > max) {
				this->Distance = max;
			}
			this->UpdateCameraPosition();
		}

	protected:
		float Distance = 200.0f;
		
		void UpdateCameraVectors() override {
			this->Front = glm::normalize(this->Target - this->Position);
			this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
			this->Up = glm::normalize(glm::cross(this->Right, this->Front));
		}
		
	private:
		void UpdateCameraPosition() {
			glm::vec4 radius = glm::vec4(0.0f, 0.0f, this->Distance, 1.0f);

			glm::mat4 rotateMatrix = glm::mat4(1.0f);
			rotateMatrix = glm::rotate(rotateMatrix, glm::radians(-this->Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
			rotateMatrix = glm::rotate(rotateMatrix, glm::radians(this->Pitch), glm::vec3(1.0f, 0.0f, 0.0f));

			glm::mat4 translation = glm::translate(glm::mat4(1.0f), this->Target);

			glm::vec4 pos = translation * rotateMatrix * radius;

			this->Position = glm::vec3(pos.x, pos.y, pos.z);
			if(this->Restrict) {
				this->Position = glm::clamp(this->Position, this->RestrictMin, this->RestrictMax);
			}

			// 算出攝影機的位置之後，就能算出攝影機的座標軸
			this->UpdateCameraVectors();
		}
	};
}