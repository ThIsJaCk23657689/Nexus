#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "Application.h"

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <random>

#define PI 3.14159265359f

class NexusDemo final : public Nexus::Application {
public:
	NexusDemo() {
		Settings.Width = 1280;
		Settings.Height = 720;
		Settings.WindowTitle = "NexusDemo | Nexus";
		Settings.EnableDebugCallback = true;
		Settings.EnableCursor = true;
	}

	void Initialize() override {
		
		// Setting OpenGL
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Create shader program

		// Create object data

		// Loading textures

		// Initial Light Setting
	}

	void Update() override {
		
	}

	void OnWindowResize(int width, int height) override {

	}
	
	void OnProcessInput(int key) override {

	}
	
	void OnKeyPress(int key) override {

	}
	
	void OnKeyRelease(int key) override {

	}
	
	void OnMouseMove(int xoffset, int yoffset) override {

	}
	
	void OnMouseButtonPress(int button) override {

	}
	
	void OnMouseButtonRelease(int button) override {

	}
	
	void OnMouseScroll(int yoffset) override {

	}

private:
	
};

int main() {
	NexusDemo app;
	return app.Run();
}