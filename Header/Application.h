#pragma once

#include "MessageLogger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace Nexus {

	struct ApplicationSettings {
		std::string WindowTitle = "Nexus";
		int Width = 1280;
		int Height = 720;
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
		virtual void OnProcessInput(int key) {}
		virtual void OnKeyPress(int key) {}
		virtual void OnKeyRelease(int key) {}
		virtual void OnMouseMove(int xoffset, int yoffset) {}
		virtual void OnMouseButtonPress(int button) {}
		virtual void OnMouseButtonRelease(int button) {}
		virtual void OnMouseScroll(int yoffset) {}

		GLFWwindow* Window = nullptr;
		
		float DeltaTime = 0.0f;
		float LastTime = 0.0f;

		MessageLogger Logger;
		ApplicationSettings Settings;
		
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
		float lastX = 0.0f;
		float lastY = 0.0f;
	};
	
}
