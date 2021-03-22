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
#include "FileLoader.h"

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <map>
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
		camera = std::make_unique<Nexus::Camera>(glm::vec3(0.0f, 2.0f, 10.0f));

		// Create object data
		triangle = std::make_unique<Nexus::Triangle>();
		triangle->setWireFrameMode(true);
		
		square = std::make_unique<Nexus::Square>();
		// square->setWireFrameMode(true);
		
		// Loading textures

		// Initial Light Setting
		


		std::map<unsigned int, unsigned int> histogram;
		std::vector<unsigned char> Testing = Nexus::FileLoader::load("C:/Users/user/Desktop/Scalar/engine.raw");
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
		
		
	}
	
	void Update() override {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		triangle->Draw(myShader.get());
		square->setColor(glm::vec3(0.5, 0.1, 0.9));
		square->Draw(myShader.get());
		
		// ImGui::ShowDemoWindow();
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
	std::unique_ptr<Nexus::Shader> myShader = nullptr;
	std::unique_ptr<Nexus::Camera> camera = nullptr;
	std::unique_ptr<Nexus::Triangle> triangle = nullptr;
	std::unique_ptr<Nexus::Square> square = nullptr;
};

int main() {
	NexusDemo app;
	return app.Run();
}