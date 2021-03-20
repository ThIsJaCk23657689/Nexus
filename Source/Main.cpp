#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "Application.h"
#include "Camera.h"
#include "Shader.h"
#include "Triangle.h"
#include "Square.h"
#include "Cube.h"
#include "FileLoader.h"

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <random>

#include "Logger.h"


#define PI 3.14159265359f

class NexusDemo final : public Nexus::Application {
public:
	NexusDemo() {
		Settings.Width = 800;
		Settings.Height = 600;
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
		myShader = std::make_unique<Nexus::Shader>("Shaders/testing.vert", "Shaders/testing.frag");
		
		// Create Camera
		camera = std::make_unique<Nexus::Camera>(glm::vec3(0.0f, 0.0f, 5.0f));

		// Create object data
		triangle = std::make_unique<Nexus::Triangle>();
		// triangle->SetWireFrameMode(true);
		
		square = std::make_unique<Nexus::Square>();
		// square->SetWireFrameMode(true);

		cube = std::make_unique<Nexus::Cube>();
		// cube->Debug();
		
		// Loading textures

		// Initial Light Setting
		

		/*
		std::unordered_map<unsigned int, unsigned int> histogram;
		std::vector<unsigned char> Testing = Nexus::FileLoader::Load("C:/Users/y3939/Desktop/OpenGL/Scalar/engine.raw");
		for (unsigned int i = 0; i < Testing.size(); i++) {
			if (histogram.find(Testing[i]) != histogram.end()) {
				histogram[Testing[i]]++;
			} else {
				histogram[Testing[i]] = 1;
			}
		}

		unsigned int total = 0;
		for (auto it : histogram) {
			std::cout << " " << it.first << ":" << it.second << std::endl;
			total += it.second;
		}
		std::cout << "Total:" << total << std::endl;
		*/
		
	}
	
	void Update() override {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(camera->GetFOV()), (float)(Settings.Width / Settings.Height), 0.1f, 100.0f);
		view = camera->GetViewMatrix();
		myShader->SetMat4("projection", projection);
		myShader->SetMat4("view", view);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
		myShader->SetMat4("model", model);
		triangle->SetColor(glm::vec3(0.1, 0.8, 0.2));
		triangle->Draw(myShader.get());

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		myShader->SetMat4("model", model);
		square->SetColor(glm::vec3(0.9, 0.1, 0.2));
		square->Draw(myShader.get());

		model = glm::mat4(1.0f);
		model = glm::rotate(model, CurrentTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		myShader->SetMat4("model", model);
		cube->SetColor(glm::vec3(0.2, 0.2, 0.9));
		cube->Draw(myShader.get());
		

		// ImGui::ShowDemoWindow();
	}
	
	void OnProcessInput(int key) override {
		if (key == GLFW_KEY_W) {
			camera->ProcessKeyboard(Nexus::CAMERA_FORWARD, DeltaTime);
		}
		if (key == GLFW_KEY_S) {
			camera->ProcessKeyboard(Nexus::CAMERA_BACKWARD, DeltaTime);
		}
		if (key == GLFW_KEY_A) {
			camera->ProcessKeyboard(Nexus::CAMERA_LEFT, DeltaTime);
		}
		if (key == GLFW_KEY_D) {
			camera->ProcessKeyboard(Nexus::CAMERA_RIGHT, DeltaTime);
		}
	}
	
	void OnKeyPress(int key) override {
		if (key == GLFW_KEY_LEFT_SHIFT) {
			camera->SetMovementSpeed(25.0f);
		}
	}
	
	void OnKeyRelease(int key) override {
		if (key == GLFW_KEY_LEFT_SHIFT) {
			camera->SetMovementSpeed(10.0f);
		}
	}
	
	void OnMouseMove(int xoffset, int yoffset) override {
		if (!Settings.EnableCursor) {
			camera->ProcessMouseMovement(xoffset, yoffset);
		}
	}
	
	void OnMouseButtonPress(int button) override {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			SetCursorDisable(true);
			Settings.EnableCursor = false;
		} 
	}
	
	void OnMouseButtonRelease(int button) override {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			SetCursorDisable(false);
			Settings.EnableCursor = true;
		}
	}
	
	void OnMouseScroll(int yoffset) override {
		camera->ProcessMouseScroll(yoffset);
	}

private:
	std::unique_ptr<Nexus::Shader> myShader = nullptr;
	std::unique_ptr<Nexus::Camera> camera = nullptr;

	std::unique_ptr<Nexus::Triangle> triangle = nullptr;
	std::unique_ptr<Nexus::Square> square = nullptr;
	std::unique_ptr<Nexus::Cube> cube = nullptr;

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
};

int main() {
	NexusDemo app;
	return app.Run();
}