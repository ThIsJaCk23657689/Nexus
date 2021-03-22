#include "Camera.h"

namespace Nexus {

	Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		this->Set(position, up, yaw, pitch);
	}

	Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		this->Set(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch);
	}
	
	void Camera::ProcessKeyboard(CameraMovement direction, float delta_time) {
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
	
	void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
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
	void Camera::Set(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->UpdateCameraVectors();
	}
	
	void Camera::SetPosition(glm::vec3 position) {
		this->Set(position, this->WorldUp, this->Yaw, this->Pitch);
	}
	
	void Camera::SetWorldUp(glm::vec3 world_up) {
		this->Set(this->Position, world_up, this->Yaw, this->Pitch);
	}
	
	void Camera::SetYaw(float yaw) {
		this->Set(this->Position, this->WorldUp, yaw, this->Pitch);
	}
	
	void Camera::SetPitch(float pitch) {
		this->Set(this->Position, this->WorldUp, this->Yaw, pitch);
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