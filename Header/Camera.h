#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Nexus {
	
	enum CameraMovement {
		CAMERA_FORWARD,
		CAMERA_BACKWARD,
		CAMERA_LEFT,
		CAMERA_RIGHT
	};

	const float YAW = 0.0f;
	const float PITCH = 0.0f;
	const float SPEED = 10.0f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
	
	class Camera {
	public:
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
		virtual ~Camera() {}

		void ProcessKeyboard(CameraMovement direction, float delta_time);
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
		void ProcessMouseScroll(float yoffset);

		// Getter
		glm::mat4 GetViewMatrix();
		glm::vec3 GetPosition() const { return this->Position; }
		glm::vec3 GetFront() const { return this->Front; }
		glm::vec3 GetUp() const { return this->Up; }
		glm::vec3 GetRight() const { return this->Right; }
		float GetYaw() const { return this->Yaw; }
		float GetPitch() const { return this->Pitch; }
		float GetMovementSpeed() const { return this->MovementSpeed; }
		float GetMouseSensitivity() const { return this->MouseSensitivity; }
		float GetFOV() const { return this->Zoom; }
		
		// Setter
		void Set(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
		void SetPosition(glm::vec3 position);
		void SetWorldUp(glm::vec3 world_up);
		void SetYaw(float yaw);
		void SetPitch(float pitch);
		void SetMovementSpeed(float speed);
		void SetMouseSensitivity(float sensitivity);
		void SetZoom(float zoom);

	private:
		glm::vec3 Position;
		
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		float Yaw;
		float Pitch;
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		void UpdateCameraVectors();
		glm::mat4 CalcLookAtMatrix(glm::vec3 positon, glm::vec3 target, glm::vec3 world_up) const;
	};
}