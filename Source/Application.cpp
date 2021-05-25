#include "Application.h"
#include "Logger.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <iostream>

namespace Nexus {

	void Application::InitializeBase() {

		Logger::ShowMe();

		// Initialize GLFW
		if (!glfwInit()) {
			Logger::Message(LOG_ERROR, "Oops! Failed to initialize GLFW. :(");
			glfwTerminate();
			exit(-1);
		}
		Logger::Message(LOG_DEBUG, "Initialize GLFW successfully.");

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 8);

		if (Settings.EnableDebugCallback) {
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		}

		// Create a window
		Window = glfwCreateWindow(Settings.Width, Settings.Height, Settings.WindowTitle.c_str(), nullptr, nullptr);
		if (!Window) {
			Logger::Message(LOG_ERROR, "Oops! Failed to create a GLFW window. :(");
			glfwTerminate();
			exit(-1);
		}
		Logger::Message(LOG_DEBUG, "Create a GLFW window successfully.");

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
			Logger::Message(LOG_ERROR, "Failed to initialize GLAD.");
			glfwTerminate();
			exit(-1);
		}
		Logger::Message(LOG_DEBUG, "Initialize GLAD successfully.");

		// Initialize ImGui and bind to GLFW and OpenGL3(glad)
		std::string glsl_version = "#version 330";
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiSettings.ImGui_IO = &ImGui::GetIO();
		ImGui_ImplGlfw_InitForOpenGL(Window, true);
		ImGui_ImplOpenGL3_Init(glsl_version.c_str());
		ImGui::StyleColorsDark();
		ImGuiSettings.ImGui_IO->Fonts->AddFontFromFileTTF(ImGuiSettings.StringFontPath.c_str(), ImGuiSettings.FontSize);

		// Mouse Initialize
		LastX = static_cast<float>(Settings.Width / 2.0f);
		LastY = static_cast<float>(Settings.Height / 2.0f);
	}
	
	int Application::Run() {
		
		InitializeBase();
		Initialize();

		// Show version info
		const GLubyte* OpenGLVender = glGetString(GL_VENDOR);
		const GLubyte* OpenGLRenderer = glGetString(GL_RENDERER);
		const GLubyte* OpenGLVersion = glGetString(GL_VERSION);
		Logger::ShowInitInfo(OpenGLVender, OpenGLRenderer, OpenGLVersion);

		while(!glfwWindowShouldClose(Window)) {

			// ImGUI Event Detect
			ImGuiSettings.SkipApplicationIO = false;
			if (ImGuiSettings.ImGui_IO->WantCaptureKeyboard || ImGuiSettings.ImGui_IO->WantCaptureMouse) {
				ImGuiSettings.SkipApplicationIO = true;
			}

			// Calculate the delta time
			CurrentTime = (float)glfwGetTime();
			DeltaTime = CurrentTime - LastTime;
			LastTime = CurrentTime;

			// Process Input (Moving camera)
			GLFWProcessInput(Window);

			// Clear the buffer
			glClearColor(this->Settings.BackgroundColor.r, this->Settings.BackgroundColor.g, this->Settings.BackgroundColor.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ShowDebugUI();

			// Display Default (DISPLAY_MODE_DEFAULT)
			int scr_start = 4, scr_end = 4;
			if (Settings.CurrentDisplyMode == DISPLAY_MODE_3O1P) {
				scr_start = 1;
				scr_end = 4;
			} else if (Settings.CurrentDisplyMode == DISPLAY_MODE_ORTHOGONAL_X) {
				scr_start = 1;
				scr_end = 1;
			} else if (Settings.CurrentDisplyMode == DISPLAY_MODE_ORTHOGONAL_Y) {
				scr_start = 2;
				scr_end = 2;
			} else if (Settings.CurrentDisplyMode == DISPLAY_MODE_ORTHOGONAL_Z) {
				scr_start = 3;
				scr_end = 3;
			}
			
			for (int i = scr_start; i <= scr_end; i++) {
				Update(static_cast<DisplayMode>(i));
			}

			// Render imgui on the screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Swap Buffers and Trigger event
			glfwSwapBuffers(Window);
			glfwPollEvents();
		}
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
		
		return 0;
	}

	void Application::SetCursorDisable(bool enable) {
		if (enable) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void Application::SetViewport(Nexus::DisplayMode monitor_type) {
		glViewport(0, 0, Settings.Width, Settings.Height);
	}

	void Application::GLFWFrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
		// Set new width and height
		Settings.Width = width;
		Settings.Height = height;
		OnWindowResize();
	}

	void Application::GLFWProcessInput(GLFWwindow* window) {
		if (ImGuiSettings.SkipApplicationIO) {
			return;
		}
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
		if (ImGuiSettings.SkipApplicationIO) {
			return;
		}
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
		if (ImGuiSettings.SkipApplicationIO) {
			return;
		}
		// In the first time u create a window, ur cursor may not in the middle of the window.
		if (FirstMouse) {
			LastX = xpos;
			LastY = ypos;
			FirstMouse = false;
		}
		
		float xoffset = xpos - LastX;
		float yoffset = LastY - ypos;

		LastX = (float)xpos;
		LastY = (float)ypos;
		
		OnMouseMove(xoffset, yoffset);
	}
	
	void Application::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (ImGuiSettings.SkipApplicationIO) {
			return;
		}
		if (action == GLFW_PRESS) {
			OnMouseButtonPress(button);
		} else if (action == GLFW_RELEASE) {
			OnMouseButtonRelease(button);
		}
	}
	
	void Application::GLFWScrollCallback(GLFWwindow* window, double xpos, double ypos) {
		if(ImGuiSettings.SkipApplicationIO) {
			return;
		}
		OnMouseScroll((float)ypos);
	}

	glm::mat4 Application::GetPerspectiveProjMatrix(float fovy, float ascept, float znear, float zfar) const {
		glm::mat4 proj = glm::mat4(1.0f);

		proj[0][0] = 1 / (tan(fovy / 2) * ascept);
		proj[1][0] = 0;
		proj[2][0] = 0;
		proj[3][0] = 0;

		proj[0][1] = 0;
		proj[1][1] = 1 / (tan(fovy / 2));
		proj[2][1] = 0;
		proj[3][1] = 0;

		proj[0][2] = 0;
		proj[1][2] = 0;
		proj[2][2] = -(zfar + znear) / (zfar - znear);
		proj[3][2] = (-2 * zfar * znear) / (zfar - znear);

		proj[0][3] = 0;
		proj[1][3] = 0;
		proj[2][3] = -1;
		proj[3][3] = 0;

		return proj;
	}
	
	glm::mat4 Application::GetOrthoProjMatrix(float left, float right, float bottom, float top, float near, float far) const {
		glm::mat4 proj = glm::mat4(1.0f);

		proj[0][0] = 2 / (right - left);
		proj[1][0] = 0;
		proj[2][0] = 0;
		proj[3][0] = -(right + left) / (right - left);

		proj[0][1] = 0;
		proj[1][1] = 2 / (top - bottom);
		proj[2][1] = 0;
		proj[3][1] = -(top + bottom) / (top - bottom);

		proj[0][2] = 0;
		proj[1][2] = 0;
		proj[2][2] = -2 / (far - near);
		proj[3][2] = -(far + near) / (far - near);

		proj[0][3] = 0;
		proj[1][3] = 0;
		proj[2][3] = 0;
		proj[3][3] = 1;

		return proj;
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
