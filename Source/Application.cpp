#include "Application.h"

#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Nexus {

	int Application::Run() {
		InitializeBase();
		Initialize();

		while(!glfwWindowShouldClose(Window)) {

			// Calculate the delta time
			float currentTime = (float)glfwGetTime();
			DeltaTime = currentTime - LastTime;
			LastTime = currentTime;

			// Process Input (Moving camera)
			GLFWProcessInput(Window);

			// Feed inputs to dear imgui start new frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Display
			Update();

			// Render imgui on the screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Swap Buffers and Trigger event
			glfwSwapBuffers(Window);
			glfwPollEvents();
		}

		return 0;
	}

	void Application::GLFWFrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		OnWindowResize(width, height);
	}

	void Application::GLFWProcessInput(GLFWwindow* window) {
		// Handle camera moving
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			OnProcessInput(GLFW_KEY_W);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			OnProcessInput(GLFW_KEY_S);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			OnProcessInput(GLFW_KEY_A);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			OnProcessInput(GLFW_KEY_D);
		}
	}
	
	void Application::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
			
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			OnKeyPress(key);
		} else if (action == GLFW_RELEASE) {
			OnKeyRelease(key);
		}
	}
	
	void Application::GLFWMouseCallback(GLFWwindow* window, double xpos, double ypos) {
		// In the first time u create a window, ur cursor may not in the middle of the window.
		if (FirstMouse) {
			lastX = xpos;
			lastY = ypos;
			FirstMouse = false;
		}
		
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		
		OnMouseMove(xoffset, yoffset);
		
		lastX = (float)xpos;
		lastY = (float)ypos;
	}
	
	void Application::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS) {
			OnMouseButtonPress(button);
		} else if (action == GLFW_RELEASE) {
			OnMouseButtonRelease(button);
		}
	}
	
	void Application::GLFWScrollCallback(GLFWwindow* window, double xpos, double ypos) {
		OnMouseScroll((float)ypos);
	}


	void Application::InitializeBase() {
		
		// Initialize GLFW
		if (!glfwInit()) {
			Logger.Message(LOG_ERROR, "Failed to initialize GLFW.");
			glfwTerminate();
			exit(-1);
		}
		Logger.Message(LOG_DEBUG, "Initialize GLFW successful.");
		
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 32);

		if(Settings.EnableDebugCallback) {
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		}

		// Create a window
		Window = glfwCreateWindow(Settings.Width, Settings.Height, Settings.WindowTitle.c_str(), nullptr, nullptr);
		if (!Window) {
			Logger.Message(LOG_ERROR, "Failed to create GLFW window :(");
			glfwTerminate();
			exit(-1);
		}
		Logger.Message(LOG_DEBUG, "Create GLFW window successful.");

		// Register callbacks and settings
		glfwMakeContextCurrent(Window);
		glfwSetWindowUserPointer(Window, this);
		glfwSetFramebufferSizeCallback(Window, GLFWFrameBufferSizeCallbackHelper);
		glfwSetKeyCallback(Window, GLFWKeyCallbackHelper);
		glfwSetCursorPosCallback(Window, GLFWMouseCallbackHelper);
		glfwSetMouseButtonCallback(Window, GLFWMouseButtonCallbackHelper);
		glfwSetScrollCallback(Window, GLFWScrollCallbackHelper);
		glfwSetInputMode(Window, GLFW_CURSOR, Settings.EnableCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

		// Initialize GLAD (Must behind the create window)
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			Logger.Message(LOG_ERROR, "Failed to initialize GLAD.");
			glfwTerminate();
			exit(-1);
		}
		Logger.Message(LOG_DEBUG, "Initialize GLAD successful.");

		// Initialize ImGui and bind to GLFW and OpenGL3(glad)
		std::string glsl_version = "#version 330";
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		// ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplGlfw_InitForOpenGL(Window, true);
		ImGui_ImplOpenGL3_Init(glsl_version.c_str());

		// Show version info
		const GLubyte* opengl_vendor = glGetString(GL_VENDOR);
		const GLubyte* opengl_renderer = glGetString(GL_RENDERER);
		const GLubyte* opengl_version = glGetString(GL_VERSION);
		Logger.ShowInitInfo(opengl_vendor, opengl_renderer, opengl_version);

		// Mouse Initialize
		lastX = (float)Settings.Width / 2.0f;
		lastY = (float)Settings.Height / 2.0f;
		
	}
	
	void Application::GLFWFrameBufferSizeCallbackHelper(GLFWwindow* window, int width, int height) {
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->GLFWFrameBufferSizeCallback(window, width, height);
	}
	
	void Application::GLFWKeyCallbackHelper(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->GLFWKeyCallback(window, key, scancode, action, mods);
	}
	
	void Application::GLFWMouseCallbackHelper(GLFWwindow* window, double xpos, double ypos) {
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->GLFWMouseCallback(window, xpos, ypos);
	}
	
	void Application::GLFWMouseButtonCallbackHelper(GLFWwindow* window, int button, int action, int mods) {
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->GLFWMouseButtonCallback(window, button, action, mods);
	}
	
	void Application::GLFWScrollCallbackHelper(GLFWwindow* window, double xpos, double ypos) {
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->GLFWScrollCallback(window, xpos, ypos);
	}
}