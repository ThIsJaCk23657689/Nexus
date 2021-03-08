#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace Nexus {
	struct ApplicationSettings {
		std::string WindowTitle = "Nexus";
		unsigned int Width = 1280;
		unsigned int Height = 720;
		std::vector <int> WindowPosition{ 0, 0 };
		std::vector <int> WindowSize{ 0, 0 };
		bool EnableCursor = true;
		bool EnableFullScreen = false;
		bool EnableDebugCallback = true;
		bool EnableGhostMode = false;
		bool ShowAxis = false;
	};

	struct Application {
		virtual void Initialize() = 0;
		virtual void Update() = 0;

		virtual ~Application() {}

		int Run();

	protected:
		virtual void OnWindowResize(int width, int height) {}
		virtual void OnKeyPress(int key_code) {}
		virtual void OnKeyRelease(int key_code) {}
		virtual void OnMouseMove(int x_offset, int y_offset) {}
		virtual void OnMouseScroll(int vertical_offset) {}

		GLFWwindow* Window = nullptr;
		float DeltaTime = 0.0f;

		ApplicationSettings Settings;
		
	private:
		void InitializeBase();

		void GLFWFrameBufferSizeCallback(GLFWwindow* window, int width, int height);
		void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void GLFWMouseCallback(GLFWwindow* window, double xpos, double ypos);
		void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void GLFWScrollCallback(GLFWwindow* window, double xpos, double ypos);
		void GLFWErrorCallback(int error, const char* description);
		void GLFWProcessInput(GLFWwindow* window);

		static void GLFWFrameBufferSizeCallbackHelper(GLFWwindow* window, int width, int height);
		static void GLFWKeyCallbackHelper(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLFWMouseCallbackHelper(GLFWwindow* window, double xpos, double ypos);
		static void GLFWMouseButtonCallbackHelper(GLFWwindow* window, int button, int action, int mods);
		static void GLFWScrollCallbackHelper(GLFWwindow* window, double xpos, double ypos);
		static void GLFWErrorCallbackHelper(int error, const char* description);
		static void GLFWProcessInputHelper(GLFWwindow* window);
		
		bool FirstMouse = true;
		float lastX = (float) Settings.Width / 2.0f;
		float lastY = (float) Settings.Height / 2.0f;
	};
}
