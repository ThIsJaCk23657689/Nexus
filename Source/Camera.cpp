#include "Camera.h"
#include <imgui.h>

namespace Nexus {

	Camera::Camera() : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
	}
	
	void Camera::ProcessMouseScroll(float yoffset) {
		if (this->Zoom >= 1.0f && this->Zoom <= 90.0f) {
			this->Zoom -= yoffset;
		}
		if (this->Zoom < 1.0f) {
			this->Zoom = 1.0f;
		}
		if (this->Zoom > 90.0f) {
			this->Zoom = 90.0f;
		}
	}

	// Getter
	glm::mat4 Camera::GetViewMatrix() {
		return this->CalcLookAtMatrix(this->Position, this->Position + this->Front, this->WorldUp);
	}

	// Setter
	void Camera::SetRestrict(bool restrict) {
		this->Restrict = restrict;
	}

	void Camera::SetRestrictValue(glm::vec3 min, glm::vec3 max) {
		this->RestrictMax = max - glm::vec3(0.2f);
		this->RestrictMin = min + glm::vec3(0.2f);
	}
	
	void Camera::SetMovementSpeed(float speed) {
		this->MovementSpeed = speed;
	}
	
	void Camera::SetMouseSensitivity(float sensitivity) {
		this->MouseSensitivity = sensitivity;
	}
	
	void Camera::SetZoom(float zoom) {
		this->Zoom = zoom;
	}

	void Camera::ShowDebugUI(const std::string& camera_name) {
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), "%s", camera_name.c_str());
		ImGui::Text("Position = (%.2f, %.2f, %.2f)", this->GetPosition().x, this->GetPosition().y, this->GetPosition().z);
		ImGui::Text("Target = (%.2f, %.2f, %.2f)", this->GetTarget().x, this->GetTarget().y, this->GetTarget().z);
		ImGui::Text("Front = (%.2f, %.2f, %.2f)", this->GetFront().x, this->GetFront().y, this->GetFront().z);
		ImGui::Text("Right = (%.2f, %.2f, %.2f)", this->GetRight().x, this->GetRight().y, this->GetRight().z);
		ImGui::Text("Up = (%.2f, %.2f, %.2f)", this->GetUp().x, this->GetUp().y, this->GetUp().z);
		ImGui::Text("Pitch = %.2f deg, Yaw = %.2f deg", this->GetPitch(), this->GetYaw());
	}
	
	void Camera::UpdateCameraVectors() {
		glm::mat4 rotateMatrix = glm::mat4(1.0f);
		rotateMatrix = glm::rotate(rotateMatrix, glm::radians(-this->Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		rotateMatrix = glm::rotate(rotateMatrix, glm::radians(this->Pitch), glm::vec3(1.0f, 0.0f, 0.0f));

		// Camera always face to negative Z.
		glm::vec4 front = rotateMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

		// Gram-Schmidt Orthogonalization
		this->Front = glm::normalize(glm::vec3(front.x, front.y, front.z));
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));

		this->Target = this->Position + this->Front;
	}
	
	glm::mat4 Camera::CalcLookAtMatrix(glm::vec3 positon, glm::vec3 target, glm::vec3 world_up) const {
		// Notice the zaxis faces to positive z.
		glm::vec3 zaxis = glm::normalize(positon - target);
		glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(this->WorldUp), zaxis));
		glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));

		glm::mat4 translation = glm::mat4(1.0f);
		translation[3][0] = -positon.x;
		translation[3][1] = -positon.y;
		translation[3][2] = -positon.z;

		glm::mat4 rotation = glm::mat4(1.0f);
		rotation[0][0] = xaxis.x;
		rotation[1][0] = xaxis.y;
		rotation[2][0] = xaxis.z;
		rotation[0][1] = yaxis.x;
		rotation[1][1] = yaxis.y;
		rotation[2][1] = yaxis.z;
		rotation[0][2] = zaxis.x;
		rotation[1][2] = zaxis.y;
		rotation[2][2] = zaxis.z;

		return rotation * translation;
	}
}
