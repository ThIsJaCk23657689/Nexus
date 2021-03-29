#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "Application.h"
#include "Logger.h"
#include "Camera.h"
#include "Shader.h"
#include "MatrixStack.h"
#include "FileLoader.h"

#include "Cube.h"
#include "Sphere.h"

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <map>
#include <random>

#define RESOLUTION_X 149
#define RESOLUTION_Y 208
#define RESOLUTION_Z 110
#define PI 3.14159265359f

struct Triangle {
	glm::vec3 position[3];
	glm::vec3 normal[3];
};

struct GridCell {
	glm::vec3 position[8];
	glm::vec3 normal[8];
	double value[8];
};

class VolumeRendering final : public Nexus::Application {
public:
	VolumeRendering() {
		Settings.Width = 800;
		Settings.Height = 600;
		Settings.WindowTitle = "VolumeRendering | Nexus";
		Settings.EnableDebugCallback = true;
		Settings.EnableFullScreen = false;

		Settings.EnableGhostMode = true;
		Settings.ShowOriginAnd3Axes = false;

		// Projection Settings Initalize
		ProjectionSettings.IsPerspective = false;
		ProjectionSettings.ClippingNear = 0.0001f;
		ProjectionSettings.ClippingFar = 50000.0f;
		ProjectionSettings.AspectHW = (float)Settings.Height / (float)Settings.Width;
		ProjectionSettings.AspectHW = (float)Settings.Width / (float)Settings.Height;
	}

	void Initialize() override {

		// Setting OpenGL
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Create shader program
		// myShader = std::make_unique<Nexus::Shader>("Shaders/testing.vert", "Shaders/testing.frag");
		// myShader = std::make_unique<Nexus::Shader>("Shaders/lighting.vert", "Shaders/no_lighting.frag");
		myShader = std::make_unique<Nexus::Shader>("Shaders/simple_lighting.vert", "Shaders/simple_lighting.frag");
		// instanceShader = std::make_unique<Nexus::Shader>("Shaders/instance.vert", "Shaders/lighting.frag");
		normalShader = std::make_unique<Nexus::Shader>("Shaders/normal_visualization.vs", "Shaders/normal_visualization.fs", "Shaders/normal_visualization.gs");

		// Create Camera
		camera = std::make_unique<Nexus::Camera>(glm::vec3(0.0f, 10.0f, 100.0f));

		// Create Matrix Stack
		model = std::make_unique<Nexus::MatrixStack>();

		// Create object data
		cube = std::make_unique<Nexus::Cube>();
		// cube->Debug();

		sphere = std::make_unique<Nexus::Sphere>();
		// sphere->Debug();

		// Loading Volume Data
		std::map<unsigned int, unsigned int> histogram;
		std::vector<unsigned char> Testing = Nexus::FileLoader::Load("C:/Users/user/Desktop/OpenGL/Scalar/engine.raw");
		
		for (unsigned int i = 0; i < Testing.size(); i++) {
			if (histogram.find(Testing[i]) != histogram.end()) {
				histogram[Testing[i]]++;
			} else {
				histogram[Testing[i]] = 1;
			}
		}
		int current_isolevel = 80;


		// Compute the gradient as normal of the facets
		for (int i = 0; i < RESOLUTION_Z; i++) {
			for (int j = 0; j < RESOLUTION_Y; j++) {
				for (int k = 0; k < RESOLUTION_X; k++) {

					float norm_x = 0;
					float norm_y = 0;
					float norm_z = 0;
					if (k + 1 == RESOLUTION_X) {
						// Backward difference
						norm_x = ((float)Testing[ThreeToOneDimension(k, j, i)] - (float)Testing[ThreeToOneDimension(k - 1, j, i)]) / 1.0f;
					}
					else if (k - 1 < 0) {
						// Forward difference
						norm_x = ((float)Testing[ThreeToOneDimension(k + 1, j, i)] - (float)Testing[ThreeToOneDimension(k, j, i)]) / 1.0f;
					}
					else {
						// Central difference
						norm_x = ((float)Testing[ThreeToOneDimension(k + 1, j, i)] - (float)Testing[ThreeToOneDimension(k - 1, j, i)]) / 2.0f;
					}

					if (j + 1 == RESOLUTION_Y) {
						// Backward difference
						norm_y = ((float)Testing[ThreeToOneDimension(k, j, i)] - (float)Testing[ThreeToOneDimension(k, j - 1, i)]) / 1.0f;
					}
					else if (j - 1 < 0) {
						// Forward difference
						norm_y = ((float)Testing[ThreeToOneDimension(k, j + 1, i)] - (float)Testing[ThreeToOneDimension(k, j, i)]) / 1.0f;
					}
					else {
						// Central difference
						norm_y = ((float)Testing[ThreeToOneDimension(k, j + 1, i)] - (float)Testing[ThreeToOneDimension(k, j - 1, i)]) / 2.0f;
					}

					if (i + 1 == RESOLUTION_Z) {
						// Backward difference
						norm_z = ((float)Testing[ThreeToOneDimension(k, j, i)] - (float)Testing[ThreeToOneDimension(k, j, i - 1)]) / 1.0f;
					}
					else if (i - 1 < 0) {
						// Forward difference
						norm_z = ((float)Testing[ThreeToOneDimension(k, j, i + 1)] - (float)Testing[ThreeToOneDimension(k, j, i)]) / 1.0f;
					}
					else {
						// Central difference
						norm_z = ((float)Testing[ThreeToOneDimension(k, j, i + 1)] - (float)Testing[ThreeToOneDimension(k, j, i - 1)]) / 2.0f;
					}
					glm::vec3 norm = glm::normalize(glm::vec3(norm_x, norm_y, norm_z)) * -1.0f;
					grid_cells_normals.push_back(norm);
				}
			}
		}
		
		// Input the data set and iso-value
		for(unsigned int i = 0; i < RESOLUTION_Z - 1; i++) {
			for (unsigned int j = 0; j < RESOLUTION_Y - 1; j++) {
				for (unsigned int k = 0; k < RESOLUTION_X - 1; k++) {
					
					auto gc = GridCell();
					gc.position[0] = glm::vec3(k, j, i);
					gc.value[0] = (int)Testing[ThreeToOneDimension(k, j, i)];
					gc.normal[0] = grid_cells_normals[ThreeToOneDimension(k, j, i)];

					gc.position[1] = glm::vec3(k + 1, j, i);
					gc.value[1] = (int)Testing[ThreeToOneDimension(k + 1, j, i)];
					gc.normal[1] = grid_cells_normals[ThreeToOneDimension(k + 1, j, i)];

					gc.position[2] = glm::vec3(k + 1, j, i + 1);
					gc.value[2] = (int)Testing[ThreeToOneDimension(k + 1, j, i + 1)];
					gc.normal[2] = grid_cells_normals[ThreeToOneDimension(k + 1, j, i + 1)];

					gc.position[3] = glm::vec3(k, j, i + 1);
					gc.value[3] = (int)Testing[ThreeToOneDimension(k, j, i + 1)];
					gc.normal[3] = grid_cells_normals[ThreeToOneDimension(k, j, i + 1)];

					gc.position[4] = glm::vec3(k, j + 1, i);
					gc.value[4] = (int)Testing[ThreeToOneDimension(k, j + 1, i)];
					gc.normal[4] = grid_cells_normals[ThreeToOneDimension(k, j + 1, i)];

					gc.position[5] = glm::vec3(k + 1, j + 1, i);
					gc.value[5] = (int)Testing[ThreeToOneDimension(k + 1, j + 1, i)];
					gc.normal[5] = grid_cells_normals[ThreeToOneDimension(k + 1, j + 1, i)];

					gc.position[6] = glm::vec3(k + 1, j + 1, i + 1);
					gc.value[6] = (int)Testing[ThreeToOneDimension(k + 1, j + 1, i + 1)];
					gc.normal[6] = grid_cells_normals[ThreeToOneDimension(k, j, i)];

					gc.position[7] = glm::vec3(k, j + 1, i + 1);
					gc.value[7] = (int)Testing[ThreeToOneDimension(k, j + 1, i + 1)];
					gc.normal[7] = grid_cells_normals[ThreeToOneDimension(k, j + 1, i + 1)];
					
					Polygonise(gc, current_isolevel, iso_surface_triangles);
				}
			}
		}

		glGenVertexArrays(1, &vdVAO);
		glGenBuffers(1, &vdVBO);
		glBindVertexArray(vdVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vdVBO);
		glBufferData(GL_ARRAY_BUFFER, iso_surface_vertices.size() * sizeof(float), iso_surface_vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
		
		/*
		unsigned int total = 0;
		for (auto it : histogram) {
			std::cout << " " << it.first << ":" << it.second << std::endl;
			total += it.second;
		}
		std::cout << "Total:" << total << std::endl;
		*/
	}

	void Update() override {

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SetViewMatrix(Nexus::DISPLAY_MODE_DEFAULT);
		SetProjectionMatrix(Nexus::DISPLAY_MODE_DEFAULT);

		myShader->Use();
		myShader->SetMat4("view", view);
		myShader->SetMat4("projection", projection);
		myShader->SetVec3("lightPos", camera->GetPosition());
		myShader->SetVec3("viewPos", camera->GetPosition());
		myShader->SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

		

		
		// ==================== Draw origin and 3 axes ====================
		if (Settings.ShowOriginAnd3Axes) {
			this->DrawOriginAnd3Axes(myShader.get());
		}

		/*
		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(4.0f, 1.0f, 0.0f)));
		myShader->SetVec4("material.diffuse", glm::vec4(0.0f, 1.0f, 1.0f, 1.0));
		cube->Draw(myShader.get(), model->Top());
		model->Pop();
		

		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(10.0f, 1.0f, 0.0f)));
		myShader->SetVec4("material.diffuse", glm::vec4(1.0f, 0.0f, 1.0f, 1.0));
		sphere->Draw(myShader.get(), model->Top());
		model->Pop();
		*/

		
		model->Push();
		// model->Save(glm::translate(model->Top(), glm::vec3(-RESOLUTION_X / 2.0f, -RESOLUTION_Y / 2.0f, -RESOLUTION_Z / 2.0f)));
		glBindVertexArray(vdVAO);
		myShader->SetVec3("objectColor", glm::vec3(0.482352941, 0.68627451, 0.929411765));
		myShader->SetMat4("model", model->Top());
		glDrawArrays(GL_TRIANGLES, 0, iso_surface_vertices.size());
		glBindVertexArray(0);
		model->Pop();

		/*
		normalShader->Use();
		normalShader->SetMat4("view", view);
		normalShader->SetMat4("projection", projection);
		model->Push();
		// model->Save(glm::translate(model->Top(), glm::vec3(-RESOLUTION_X / 2.0f, -RESOLUTION_Y / 2.0f, -RESOLUTION_Z / 2.0f)));
		glBindVertexArray(vdVAO);
		normalShader->SetMat4("model", model->Top());
		glDrawArrays(GL_TRIANGLES, 0, iso_surface_vertices.size());
		glBindVertexArray(0);
		model->Pop();
		*/

		// ImGui::ShowDemoWindow();
	}

	void ShowDebugUI() override {
		ImGui::Begin("Control Panel");
		ImGuiTabBarFlags tab_bar_flags = ImGuiBackendFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
			/*
			if (ImGui::BeginTabItem("ROV")) {
				ImGui::Text("Position = (%.2f, %.2f, %.2f)", ROVPosition.x, ROVPosition.y, ROVPosition.z);
				ImGui::Text("Front = (%.2f, %.2f, %.2f)", ROVFront.x, ROVFront.y, ROVFront.z);
				ImGui::Text("Right = (%.2f, %.2f, %.2f)", ROVRight.x, ROVRight.y, ROVRight.z);
				ImGui::Text("Pitch = %.2f deg", ROVYaw);
				ImGui::SliderFloat("Speed", &ROVMovementSpeed, 1, 20);
				ImGui::EndTabItem();
			}
			*/
			if (ImGui::BeginTabItem("Camera")) {
				if (Settings.EnableGhostMode) {
					ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), "Ghost Camera");
					ImGui::Text("Position = (%.2f, %.2f, %.2f)", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);
					ImGui::Text("Front = (%.2f, %.2f, %.2f)", camera->GetFront().x, camera->GetFront().y, camera->GetFront().z);
					ImGui::Text("Right = (%.2f, %.2f, %.2f)", camera->GetRight().x, camera->GetRight().y, camera->GetRight().z);
					ImGui::Text("Up = (%.2f, %.2f, %.2f)", camera->GetUp().x, camera->GetUp().y, camera->GetUp().z);
					ImGui::Text("Pitch = %.2f deg, Yaw = %.2f deg", camera->GetPitch(), camera->GetYaw());
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Projection")) {

				ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), (ProjectionSettings.IsPerspective) ? "Perspective Projection" : "Orthogonal Projection");
				ImGui::Text("Parameters");
				ImGui::BulletText("FoV = %.2f deg, Aspect = %.2f", camera->GetFOV(), ProjectionSettings.AspectWH);
				ImGui::BulletText("left: %.2f, right: %.2f ", ProjectionSettings.ClippingLeft, ProjectionSettings.ClippingTop);
				ImGui::BulletText("bottom: %.2f, top: %.2f ", ProjectionSettings.ClippingBottom, ProjectionSettings.ClippingTop);
				ImGui::SliderFloat("Near", &ProjectionSettings.ClippingNear, 0.1, 10);
				ImGui::SliderFloat("Far", &ProjectionSettings.ClippingFar, 10, 250);
				ImGui::Spacing();

				if (ImGui::TreeNode("Projection Matrix")) {
					SetProjectionMatrix(Nexus::DISPLAY_MODE_DEFAULT);
					glm::mat4 proj = projection;

					ImGui::Columns(4, "mycolumns");
					ImGui::Separator();
					for (int i = 0; i < 4; i++) {
						ImGui::Text("%.2f", proj[0][i]); ImGui::NextColumn();
						ImGui::Text("%.2f", proj[1][i]); ImGui::NextColumn();
						ImGui::Text("%.2f", proj[2][i]); ImGui::NextColumn();
						ImGui::Text("%.2f", proj[3][i]); ImGui::NextColumn();
						ImGui::Separator();
					}
					ImGui::Columns(1);

					ImGui::TreePop();
				}
				ImGui::Spacing();
				/*
				if (ImGui::TreeNode("View Volume Vertices")) {
					ImGui::BulletText("rtnp: (%.2f, %.2f, %.2f)", nearPlaneVertex[0].x, nearPlaneVertex[0].y, nearPlaneVertex[0].z);
					ImGui::BulletText("ltnp: (%.2f, %.2f, %.2f)", nearPlaneVertex[1].x, nearPlaneVertex[1].y, nearPlaneVertex[1].z);
					ImGui::BulletText("rbnp: (%.2f, %.2f, %.2f)", nearPlaneVertex[2].x, nearPlaneVertex[2].y, nearPlaneVertex[2].z);
					ImGui::BulletText("lbnp: (%.2f, %.2f, %.2f)", nearPlaneVertex[3].x, nearPlaneVertex[3].y, nearPlaneVertex[3].z);
					ImGui::BulletText("rtfp: (%.2f, %.2f, %.2f)", farPlaneVertex[0].x, farPlaneVertex[0].y, farPlaneVertex[0].z);
					ImGui::BulletText("ltfp: (%.2f, %.2f, %.2f)", farPlaneVertex[1].x, farPlaneVertex[1].y, farPlaneVertex[1].z);
					ImGui::BulletText("rbfp: (%.2f, %.2f, %.2f)", farPlaneVertex[2].x, farPlaneVertex[2].y, farPlaneVertex[2].z);
					ImGui::BulletText("lbfp: (%.2f, %.2f, %.2f)", farPlaneVertex[3].x, farPlaneVertex[3].y, farPlaneVertex[3].z);
					ImGui::TreePop();
				}

				ImGui::Spacing();
				*/
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Illustration")) {

				// ImGui::Text("Current Screen: %d", currentScreen + 1);
				ImGui::Text("Ghost Mode: %s", Settings.EnableGhostMode ? "True" : "false");
				// ImGui::Text("Projection Mode: %s", isPerspective ? "Perspective" : "Orthogonal");
				ImGui::Text("Showing Axes: %s", Settings.ShowOriginAnd3Axes ? "True" : "false");
				// ImGui::Text("Full Screen:  %s", isfullscreen ? "True" : "false");
				// ImGui::SliderFloat("zoom", &distanceOrthoCamera, 5, 25);
				ImGui::Spacing();

				if (ImGui::TreeNode("General")) {
					ImGui::BulletText("Press G to switch Ghost Mode");
					ImGui::BulletText("Press X to show / hide the axes");
					ImGui::BulletText("Press Y to switch the projection");
					ImGui::BulletText("Press 1~5 to switch the screen");
					ImGui::BulletText("Press F11 to Full Screen");
					ImGui::BulletText("Press ESC to close the program");
					ImGui::TreePop();
				}
				ImGui::Spacing();

				if (ImGui::TreeNode("Ghost Camera Illustration")) {
					ImGui::BulletText("WSAD to move camera");
					ImGui::BulletText("Hold mouse right button to rotate");
					ImGui::BulletText("Press Left Shift to speed up");
					ImGui::TreePop();
				}
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
			/*
			if (ImGui::BeginTabItem("Texture")) {
				ImGui::Checkbox("Billboard", &enableBillboard);
				ImGui::SliderInt(std::string("Key Frame Rate").c_str(), &keyFrameRate, 0, 24);
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
			*/
			ImGui::EndTabBar();
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::End();
	}

	void DrawOriginAnd3Axes(Nexus::Shader* shader) const {

		shader->SetBool("material.enableDiffuseTexture", false);
		
		// 繪製世界坐標系原點（0, 0, 0）
		model->Push();
		model->Save(glm::scale(model->Top(), glm::vec3(0.2f, 0.2f, 0.2f)));
		shader->SetVec4("material.diffuse", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		sphere->Draw(shader, model->Top());
		model->Pop();

		// 繪製三個軸
		model->Push();
		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(1.5f, 0.0f, 0.0f)));
		model->Save(glm::scale(model->Top(), glm::vec3(3.0f, 0.1f, 0.1f)));
		shader->SetVec4("material.diffuse", glm::vec4(1.0f, 0.0f, 0.0f, 1.0));
		cube->Draw(shader, model->Top());
		model->Pop();

		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 1.5f, 0.0f)));
		model->Save(glm::scale(model->Top(), glm::vec3(0.1f, 3.0f, 0.1f)));
		shader->SetVec4("material.diffuse", glm::vec4(0.0f, 1.0f, 0.0f, 1.0));
		cube->Draw(shader, model->Top());
		model->Pop();

		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 0.0f, 1.5f)));
		model->Save(glm::scale(model->Top(), glm::vec3(0.1f, 0.1f, 3.0f)));
		shader->SetVec4("material.diffuse", glm::vec4(0.0f, 0.0f, 1.0f, 1.0));
		cube->Draw(shader, model->Top());
		model->Pop();
		model->Pop();
	}

	void SetViewMatrix(Nexus::DisplayMode monitor_type) {
		if (Settings.EnableGhostMode) {
			switch (monitor_type) {
			case Nexus::DISPLAY_MODE_ORTHOGONAL_X:
				view = glm::lookAt(camera->GetPosition() + glm::vec3(5.0, 0.0, 0.0), camera->GetPosition(), glm::vec3(0.0, 1.0, 0.0));
				break;
			case Nexus::DISPLAY_MODE_ORTHOGONAL_Y:
				view = glm::lookAt(camera->GetPosition() + glm::vec3(0.0, 5.0, 0.0), camera->GetPosition(), glm::vec3(0.0, 0.0, -1.0));
				break;
			case Nexus::DISPLAY_MODE_ORTHOGONAL_Z:
				view = glm::lookAt(camera->GetPosition() + glm::vec3(0.0, 0.0, 5.0), camera->GetPosition(), glm::vec3(0.0, 1.0, 0.0));
				break;
			case Nexus::DISPLAY_MODE_DEFAULT:
				view = camera->GetViewMatrix();
				break;
			}
		}
	}

	void SetProjectionMatrix(Nexus::DisplayMode monitor_type) {
		ProjectionSettings.AspectWH = (float)Settings.Width / (float)Settings.Height;
		ProjectionSettings.AspectHW = (float)Settings.Height / (float)Settings.Width;

		if (monitor_type == Nexus::DISPLAY_MODE_DEFAULT) {
			if (Settings.EnableGhostMode) {
				if (ProjectionSettings.IsPerspective) {
					projection = GetPerspectiveProjMatrix(glm::radians(camera->GetFOV()), ProjectionSettings.AspectWH, ProjectionSettings.ClippingNear, ProjectionSettings.ClippingFar);
				} else {
					float length = 500;
					if (Settings.Width > Settings.Height) {
						projection = GetOrthoProjMatrix(-length, length, -length * ProjectionSettings.AspectHW, length * ProjectionSettings.AspectHW, ProjectionSettings.ClippingNear, ProjectionSettings.ClippingFar);
					} else {
						projection = GetOrthoProjMatrix(-length * ProjectionSettings.AspectWH, length * ProjectionSettings.AspectWH, -length, length, ProjectionSettings.ClippingNear, ProjectionSettings.ClippingFar);
					}
				}
			}
		} else {
			if (Settings.Width > Settings.Height) {
				projection = GetOrthoProjMatrix(-100.0, 100.0, -100.0 * ProjectionSettings.AspectHW, 100.0 * ProjectionSettings.AspectHW, ProjectionSettings.ClippingNear, ProjectionSettings.ClippingFar);
			} else {
				projection = GetOrthoProjMatrix(-100.0 * ProjectionSettings.AspectWH, 100.0 * ProjectionSettings.AspectWH, -100.0, 100.0, ProjectionSettings.ClippingNear, ProjectionSettings.ClippingFar);
			}
		}
	}

	void SetViewport(Nexus::DisplayMode monitor_type) override {
		if (Settings.CurrentDisplyMode == Nexus::DISPLAY_MODE_3O1P) {
			switch (monitor_type) {
			case Nexus::DISPLAY_MODE_ORTHOGONAL_X:
				glViewport(0, Settings.Height / 2, Settings.Width / 2, Settings.Height / 2);
				break;
			case Nexus::DISPLAY_MODE_ORTHOGONAL_Y:
				glViewport(Settings.Width / 2, Settings.Height / 2, Settings.Width / 2, Settings.Height / 2);
				break;
			case Nexus::DISPLAY_MODE_ORTHOGONAL_Z:
				glViewport(0, 0, Settings.Width / 2, Settings.Height / 2);
				break;
			case Nexus::DISPLAY_MODE_DEFAULT:
				glViewport(Settings.Width / 2, 0, Settings.Width / 2, Settings.Height / 2);
				break;
			}
		}
		else {
			glViewport(0, 0, Settings.Width, Settings.Height);
		}
	}

	void OnWindowResize() override {
		ProjectionSettings.AspectWH = (float)Settings.Width / (float)Settings.Height;
		ProjectionSettings.AspectHW = (float)Settings.Height / (float)Settings.Width;

		// Reset viewport
		SetViewport(Nexus::DISPLAY_MODE_DEFAULT);
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
			camera->SetMovementSpeed(500.0f);
		}

		if (key == GLFW_KEY_X) {
			if (Settings.ShowOriginAnd3Axes) {
				Settings.ShowOriginAnd3Axes = false;
				Nexus::Logger::Message(Nexus::LOG_INFO, "World coordinate origin and 3 axes: [Hide].");
			} else {
				Settings.ShowOriginAnd3Axes = true;
				Nexus::Logger::Message(Nexus::LOG_INFO, "World coordinate origin and 3 axes: [Show].");
			}
		}

		if (key == GLFW_KEY_P) {
			if (ProjectionSettings.IsPerspective) {
				ProjectionSettings.IsPerspective = false;
				Nexus::Logger::Message(Nexus::LOG_INFO, "Projection Mode: Orthogonal");
			} else {
				ProjectionSettings.IsPerspective = true;
				Nexus::Logger::Message(Nexus::LOG_INFO, "Projection Mode: Perspective");
			}
		}
	}

	void OnKeyRelease(int key) override {
		if (key == GLFW_KEY_LEFT_SHIFT) {
			camera->SetMovementSpeed(50.0f);
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

	static unsigned int ThreeToOneDimension(unsigned int x, unsigned int y, unsigned int z) {
		return z * RESOLUTION_Y * RESOLUTION_X + (y * RESOLUTION_X + x);
	}

	int Polygonise(GridCell grid, double isolevel, std::vector<Triangle>& triangles) {
		int cubeindex;
		glm::vec3 vertlist[12];
		glm::vec3 normallist[12];

		int edge_table[256] = {
			0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
			0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
			0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
			0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
			0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
			0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
			0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
			0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
			0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
			0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
			0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
			0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
			0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
			0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
			0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
			0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
			0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
			0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
			0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
			0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
			0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
			0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
			0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
			0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
			0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
			0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
			0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
			0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
			0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
			0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
			0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
			0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
		};

		int tri_table[256][16] = {
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
			{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
			{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
			{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
			{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
			{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
			{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
			{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
			{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
			{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
			{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
			{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
			{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
			{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
			{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
			{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
			{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
			{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
			{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
			{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
			{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
			{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
			{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
			{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
			{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
			{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
			{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
			{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
			{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
			{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
			{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
			{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
			{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
			{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
			{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
			{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
			{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
			{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
			{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
			{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
			{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
			{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
			{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
			{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
			{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
			{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
			{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
			{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
			{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
			{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
			{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
			{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
			{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
			{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
			{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
			{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
			{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
			{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
			{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
			{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
			{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
			{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
			{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
			{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
			{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
			{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
			{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
			{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
			{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
			{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
			{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
			{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
			{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
			{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
			{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
			{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
			{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
			{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
			{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
			{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
			{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
			{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
			{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
			{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
			{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
			{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
			{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
			{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
			{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
			{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
			{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
			{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
			{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
			{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
			{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
			{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
			{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
		};

		
		cubeindex = 0;
		if (grid.value[0] >= isolevel) cubeindex |= 1;
		if (grid.value[1] >= isolevel) cubeindex |= 2;
		if (grid.value[2] >= isolevel) cubeindex |= 4;
		if (grid.value[3] >= isolevel) cubeindex |= 8;
		if (grid.value[4] >= isolevel) cubeindex |= 16;
		if (grid.value[5] >= isolevel) cubeindex |= 32;
		if (grid.value[6] >= isolevel) cubeindex |= 64;
		if (grid.value[7] >= isolevel) cubeindex |= 128;

		// Cube is entirely in/out of the surface
		if (edge_table[cubeindex] == 0) {
			return 0;
		}

		// Find the vertices where the surface intersects the cube
		if (edge_table[cubeindex] & 1) {
			vertlist[0] = VertexInterp(isolevel, grid.position[0], grid.position[1], grid.value[0], grid.value[1]);
			normallist[0] = VertexInterp(isolevel, grid.normal[0], grid.normal[1], grid.value[0], grid.value[1]);
		}
		if (edge_table[cubeindex] & 2) {
			vertlist[1] = VertexInterp(isolevel, grid.position[1], grid.position[2], grid.value[1], grid.value[2]);
			normallist[1] = VertexInterp(isolevel, grid.normal[1], grid.normal[2], grid.value[1], grid.value[2]);
		}
		if (edge_table[cubeindex] & 4) {
			vertlist[2] = VertexInterp(isolevel, grid.position[2], grid.position[3], grid.value[2], grid.value[3]);
			normallist[2] = VertexInterp(isolevel, grid.normal[2], grid.normal[3], grid.value[2], grid.value[3]);
		}
		if (edge_table[cubeindex] & 8) {
			vertlist[3] = VertexInterp(isolevel, grid.position[3], grid.position[0], grid.value[3], grid.value[0]);
			normallist[3] = VertexInterp(isolevel, grid.normal[3], grid.normal[0], grid.value[3], grid.value[0]);
		}
		if (edge_table[cubeindex] & 16) {
			vertlist[4] = VertexInterp(isolevel, grid.position[4], grid.position[5], grid.value[4], grid.value[5]);
			normallist[4] = VertexInterp(isolevel, grid.normal[4], grid.normal[5], grid.value[4], grid.value[5]);
		}
		if (edge_table[cubeindex] & 32) {
			vertlist[5] = VertexInterp(isolevel, grid.position[5], grid.position[6], grid.value[5], grid.value[6]);
			normallist[5] = VertexInterp(isolevel, grid.normal[5], grid.normal[6], grid.value[5], grid.value[6]);
		}
		if (edge_table[cubeindex] & 64) {
			vertlist[6] = VertexInterp(isolevel, grid.position[6], grid.position[7], grid.value[6], grid.value[7]);
			normallist[6] = VertexInterp(isolevel, grid.normal[6], grid.normal[7], grid.value[6], grid.value[7]);
		}
		if (edge_table[cubeindex] & 128) {
			vertlist[7] = VertexInterp(isolevel, grid.position[7], grid.position[4], grid.value[7], grid.value[4]);
			normallist[7] = VertexInterp(isolevel, grid.normal[7], grid.normal[4], grid.value[7], grid.value[4]);
		}
		if (edge_table[cubeindex] & 256) {
			vertlist[8] = VertexInterp(isolevel, grid.position[0], grid.position[4], grid.value[0], grid.value[4]);
			normallist[8] = VertexInterp(isolevel, grid.normal[0], grid.normal[4], grid.value[0], grid.value[4]);
		}
		if (edge_table[cubeindex] & 512) {
			vertlist[9] = VertexInterp(isolevel, grid.position[1], grid.position[5], grid.value[1], grid.value[5]);
			normallist[9] = VertexInterp(isolevel, grid.normal[1], grid.normal[5], grid.value[1], grid.value[5]);
		}
		if (edge_table[cubeindex] & 1024) {
			vertlist[10] = VertexInterp(isolevel, grid.position[2], grid.position[6], grid.value[2], grid.value[6]);
			normallist[10] = VertexInterp(isolevel, grid.normal[2], grid.normal[6], grid.value[2], grid.value[6]);
		}
		if (edge_table[cubeindex] & 2048) {
			vertlist[11] = VertexInterp(isolevel, grid.position[3], grid.position[7], grid.value[3], grid.value[7]);
			normallist[11] = VertexInterp(isolevel, grid.normal[3], grid.normal[7], grid.value[3], grid.value[7]);
		}

		// Create the triangle
		for (unsigned int i = 0; tri_table[cubeindex][i] != -1; i+=3) {

			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i]].x);
			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i]].y);
			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i]].z);

			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i]].x);
			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i]].y);
			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i]].z);

			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i + 1]].x);
			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i + 1]].y);
			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i + 1]].z);

			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i + 1]].x);
			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i + 1]].y);
			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i + 1]].z);

			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i + 2]].x);
			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i + 2]].y);
			iso_surface_vertices.push_back(vertlist[tri_table[cubeindex][i + 2]].z);

			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i + 2]].x);
			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i + 2]].y);
			iso_surface_vertices.push_back(normallist[tri_table[cubeindex][i + 2]].z);
			/*
			Triangle temp = Triangle();
			temp.position[0] = vertlist[tri_table[cubeindex][i]];
			temp.position[1] = vertlist[tri_table[cubeindex][i + 1]];
			temp.position[2] = vertlist[tri_table[cubeindex][i + 2]];

			temp.normal[0] = normallist[tri_table[cubeindex][i]];
			temp.normal[1] = normallist[tri_table[cubeindex][i + 1]];
			temp.normal[2] = normallist[tri_table[cubeindex][i + 2]];
			triangles.push_back(temp);
			*/
		}
	}

	glm::vec3 VertexInterp(double isolevel, glm::vec3 p1, glm::vec3 p2, double valp1, double valp2) {
		double mu;
		glm::vec3 p;

		if (abs(isolevel - valp1) < 0.00001) {
			return p1;
		}
		
		if (abs(isolevel - valp2) < 0.00001) {
			return p2;
		}
		
		if (abs(valp1 - valp2) < 0.00001) {
			return p1;
		}
		
		mu = (isolevel - valp1) / (valp2 - valp1);
		p.x = p1.x + mu * (p2.x - p1.x);
		p.y = p1.y + mu * (p2.y - p1.y);
		p.z = p1.z + mu * (p2.z - p1.z);

		return p;
	}
	
private:
	std::unique_ptr<Nexus::Shader> myShader = nullptr;
	std::unique_ptr<Nexus::Shader> normalShader = nullptr;
	std::unique_ptr<Nexus::Shader> instanceShader = nullptr;
	std::unique_ptr<Nexus::Camera> camera = nullptr;

	std::unique_ptr<Nexus::MatrixStack> model = nullptr;
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	std::unique_ptr<Nexus::Cube> cube = nullptr;
	std::unique_ptr<Nexus::Sphere> sphere = nullptr;

	std::vector<GridCell> grid_cells;
	std::vector<glm::vec3> grid_cells_normals;
	std::vector<float> iso_surface_vertices;
	std::vector<Triangle> iso_surface_triangles;
	std::vector<glm::vec3> iso_surface_normals;

	unsigned int vdVAO, vdVBO;
};

int main() {
	VolumeRendering app;
	return app.Run();
}