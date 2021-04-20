#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

namespace Nexus {

	enum DisplayMode {
		DISPLAY_MODE_ORTHOGONAL_X = 1,
		DISPLAY_MODE_ORTHOGONAL_Y = 2,
		DISPLAY_MODE_ORTHOGONAL_Z = 3,
		DISPLAY_MODE_DEFAULT = 4,
		DISPLAY_MODE_3O1P = 5
	};

	struct ApplicationSettings {
		std::string WindowTitle = "Nexus";
		int Width = 1280;
		int Height = 720;
		std::vector <int> WindowPosition{ 0, 0 };
		std::vector <int> WindowSize{ 0, 0 };
		bool EnableCursor = true;
		bool EnableFullScreen = false;
		bool EnableDebugCallback = true;
		bool EnableGhostMode = true;
		bool EnableBackFaceCulling = false;
		bool ShowOriginAnd3Axes = false;
		bool NormalVisualize = false;

		bool UseBlinnPhongShading = true;
		bool UseLighting = true;
		bool UseDiffuseTexture = true;
		bool UseSpecularTexture = true;
		bool UseEmission = true;
		bool UseGamma = false;
		float GammaValue = 1.0f / 2.2f;

		DisplayMode CurrentDisplyMode = DISPLAY_MODE_DEFAULT;
	};

	struct ProjectionConfig {
		bool IsPerspective = true;
		float Aspect = 0;
		float OrthogonalHeight = 200.0f;
		float ClippingNear = 0.1f;
		float ClippingFar = 250.0f;
	};

	class Application {
	public:
		virtual void Initialize() = 0;
		virtual void Update(DisplayMode monitor_type) = 0;

		void SetCursorDisable(bool enable);

		virtual ~Application() {
			glfwTerminate();
		}

		int Run();
		virtual void ShowDebugUI() {}

	protected:
		virtual void OnWindowResize() {}
		virtual void OnProcessInput(int key) {}
		virtual void OnKeyPress(int key) {}
		virtual void OnKeyRelease(int key) {}
		virtual void OnMouseMove(int xoffset, int yoffset) {}
		virtual void OnMouseButtonPress(int button) {}
		virtual void OnMouseButtonRelease(int button) {}
		virtual void OnMouseScroll(int yoffset) {}
		virtual void SetViewport(Nexus::DisplayMode monitor_type);
		glm::mat4 GetPerspectiveProjMatrix(float fovy, float ascept, float znear, float zfar) const;
		glm::mat4 GetOrthoProjMatrix(float left, float right, float bottom, float top, float near, float far) const;

		GLFWwindow* Window = nullptr;
		
		float CurrentTime = 0.0f;
		float DeltaTime = 0.0f;
		float LastTime = 0.0f;
		
		ApplicationSettings Settings;
		ProjectionConfig ProjectionSettings;
		
	private:
		void InitializeBase();

		void GLFWFrameBufferSizeCallback(GLFWwindow* window, int width, int height);
		void GLFWProcessInput(GLFWwindow* window);
		void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void GLFWMouseCallback(GLFWwindow* window, double xpos, double ypos);
		void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void GLFWScrollCallback(GLFWwindow* window, double xpos, double ypos);
		
		static void GLFWFrameBufferSizeCallbackHelper(GLFWwindow* window, int width, int height);
		static void GLFWKeyCallbackHelper(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLFWMouseCallbackHelper(GLFWwindow* window, double xpos, double ypos);
		static void GLFWMouseButtonCallbackHelper(GLFWwindow* window, int button, int action, int mods);
		static void GLFWScrollCallbackHelper(GLFWwindow* window, double xpos, double ypos);
		
		bool FirstMouse = true;
		float LastX = 0.0f;
		float LastY = 0.0f;
	};
	
}
