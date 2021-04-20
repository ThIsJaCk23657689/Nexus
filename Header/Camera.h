#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

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
		Camera();
		virtual ~Camera() = default;

		virtual void ProcessKeyboard(CameraMovement direction, float delta_time) {};
		virtual void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {};
		virtual void ProcessMouseScroll(float yoffset);

		// Getter
		virtual glm::mat4 GetViewMatrix();
		glm::vec3 GetPosition() const { return this->Position; }
		glm::vec3 GetTarget() const { return this->Target; }
		glm::vec3 GetFront() const { return this->Front; }
		glm::vec3 GetUp() const { return this->Up; }
		glm::vec3 GetRight() const { return this->Right; }
		float GetYaw() const { return this->Yaw; }
		float GetPitch() const { return this->Pitch; }
		bool GetRestrict() const { return this->Restrict; }
		float GetMovementSpeed() const { return this->MovementSpeed; }
		float GetMouseSensitivity() const { return this->MouseSensitivity; }
		float GetFOV() const { return this->Zoom; }
		
		// Setter
		virtual void SetPosition(glm::vec3 position) {};
		virtual void SetWorldUp(glm::vec3 world_up) {};
		virtual void SetYaw(float yaw) {};
		virtual void SetPitch(float pitch) {};
		void SetRestrict(bool restrict);
		void SetRestrictValue(glm::vec3 min, glm::vec3 max);
		void SetMovementSpeed(float speed);
		void SetMouseSensitivity(float sensitivity);
		void SetZoom(float zoom);

		virtual void ShowDebugUI(const char* camera_name);

	protected:
		glm::vec3 Position;
		glm::vec3 Target;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		bool Restrict;
		glm::vec3 RestrictMin;
		glm::vec3 RestrictMax;

		float Yaw;
		float Pitch;
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		virtual void UpdateCameraVectors();
		glm::mat4 CalcLookAtMatrix(glm::vec3 positon, glm::vec3 target, glm::vec3 world_up) const;
	};
}