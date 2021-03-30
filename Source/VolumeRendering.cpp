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
#include "IsoSurface.h"

#include "Cube.h"
#include "Sphere.h"

#include <random>

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
		ProjectionSettings.ClippingFar = 500.0f;
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
		engine = std::make_unique<Nexus::IsoSurface>("C:/Users/user/Desktop/OpenGL/Scalar/engine.inf", "C:/Users/user/Desktop/OpenGL/Scalar/engine.raw");
		
		// cube = std::make_unique<Nexus::Cube>();
		// cube->Debug();

		// sphere = std::make_unique<Nexus::Sphere>();
		// sphere->Debug();

		// Loading Volume Data
		/*
		std::map<unsigned int, unsigned int> histogram;
		std::vector<unsigned char> Testing = Nexus::FileLoader::Load("C:/Users/user/Desktop/OpenGL/Scalar/engine.raw");
		
		for (unsigned int i = 0; i < Testing.size(); i++) {
			if (histogram.find(Testing[i]) != histogram.end()) {
				histogram[Testing[i]]++;
			} else {
				histogram[Testing[i]] = 1;
			}
		}
		
		*/

		

		
		
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
		myShader->SetVec3("objectColor", glm::vec3(0.482352941, 0.68627451, 0.929411765));
		engine->Draw(myShader.get(), model->Top());
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
			if (ImGui::BeginTabItem("Iso Surface")) {
				ImGui::SliderFloat("Iso Value", &iso_value, 0, 255);
				if(ImGui::Button("Generate")) {
					engine->ConvertToPolygon(iso_value);
					engine->Debug();
				}
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
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
					float length = 200;
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

	std::unique_ptr<Nexus::IsoSurface> engine = nullptr;
	
	float iso_value = 80.0;
};

int main() {
	VolumeRendering app;
	return app.Run();
}