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
#include "Texture2D.h"
#include "Light.h"
#include "Fog.h"

#include "Triangle.h"
#include "Rectangle.h"
#include "Cube.h"
#include "Sphere.h"
#include "Cylinder.h"

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <map>
#include <random>

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
		// myShader = std::make_unique<Nexus::Shader>("Shaders/testing.vert", "Shaders/testing.frag");
		myShader = std::make_unique<Nexus::Shader>("Shaders/lighting.vert", "Shaders/lighting.frag");
		
		// Create Camera
		camera = std::make_unique<Nexus::Camera>(glm::vec3(0.0f, 2.0f, 10.0f));

		// Create Matrix Stack
		model = std::make_unique<Nexus::MatrixStack>();
		view = std::make_unique<Nexus::MatrixStack>();
		projection = std::make_unique<Nexus::MatrixStack>();

		// Create object data
		triangle = std::make_unique<Nexus::Triangle>();
		// triangle->SetWireFrameMode(true);

		floor = std::make_unique<Nexus::Rectangle>(200.0f, 200.0f, 25.0f, Nexus::POS_Y);
		plane = std::make_unique<Nexus::Rectangle>();
		square = std::make_unique<Nexus::Rectangle>(Nexus::POS_X);

		cube = std::make_unique<Nexus::Cube>();
		// cube->Debug();

		sphere = std::make_unique<Nexus::Sphere>();
		// sphere->Debug();

		cylinder = std::make_unique<Nexus::Cylinder>(1.0f, 0.1f, 3.0f);
		// cylinder->Debug();

		// Loading textures
		texture_sea = Nexus::Texture2D::CreateFromFile("Resource/Textures/sea.jpg", true);
		texture_sand = Nexus::Texture2D::CreateFromFile("Resource/Textures/sand.jpg", true);
		texture_grass = Nexus::Texture2D::CreateFromFile("Resource/Textures/grass.png", true);
		texture_box = Nexus::Texture2D::CreateFromFile("Resource/Textures/wooden_box.png", true);
		texture_box_spec = Nexus::Texture2D::CreateFromFile("Resource/Textures/wooden_box_specular.png", true);
		texture_fish = Nexus::Texture2D::CreateFromFile("Resource/Textures/fish.png", true);

		// Initial Light Setting
		DirLights = {
			new Nexus::DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f), true)
		};
		PointLights = {
			new Nexus::PointLight(glm::vec3(10.0f, 10.0f, 35.0f), true),
			new Nexus::PointLight(glm::vec3(-45.0f, 5.0f, 30.0f), true),
			new Nexus::PointLight(glm::vec3(38.0f, 2.0f, -40.0f), true),
			new Nexus::PointLight(glm::vec3(-50.0f, 15.0f, -45.0f), true),
			new Nexus::PointLight(glm::vec3(0.0f, 10.0f, 0.0f), true)
		};
		SpotLights = {
			new Nexus::SpotLight(camera->GetPosition(), camera->GetFront(), false)
		};

		// Fog
		fog = std::make_unique<Nexus::Fog>(glm::vec4(0.266f, 0.5f, 0.609f, 1.0f), false, 0.1f, 100.0f);
		
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

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection->Save(glm::perspective(glm::radians(camera->GetFOV()), (float)(Settings.Width / Settings.Height), 0.1f, 100.0f));
		view->Save(camera->GetViewMatrix());

		

		myShader->Use();
		myShader->SetInt("material.diffuse_texture", 0);
		myShader->SetInt("material.specular_texture", 1);
		myShader->SetInt("material.emission_texture", 2);
		myShader->SetInt("skybox", 3);

		myShader->SetMat4("view", view->Top());
		myShader->SetMat4("projection", projection->Top());
		myShader->SetVec3("viewPos", camera->GetPosition());

		
		myShader->SetBool("useBlinnPhong", Settings.UseBlinnPhongShading);
		myShader->SetBool("useSpotExponent", Settings.UseSpotExponent);
		myShader->SetBool("useLighting", Settings.UseLighting);
		myShader->SetBool("useDiffuseTexture", Settings.UseDiffuseTexture);
		myShader->SetBool("useSpecularTexture", Settings.UseSpecularTexture);
		myShader->SetBool("useEmission", Settings.UseEmission);
		myShader->SetBool("useGamma", Settings.UseGamma);
		myShader->SetFloat("GammaValue", Settings.GammaValue);
		
		for (unsigned int i = 0; i < DirLights.size(); i++) {
			myShader->SetVec3("lights[" + std::to_string(i) + "].direction", DirLights[i]->GetDirection());
			myShader->SetVec3("lights[" + std::to_string(i) + "].ambient", DirLights[i]->GetAmbient());
			myShader->SetVec3("lights[" + std::to_string(i) + "].diffuse", DirLights[i]->GetDiffuse());
			myShader->SetVec3("lights[" + std::to_string(i) + "].specular", DirLights[i]->GetSpecular());
			myShader->SetBool("lights[" + std::to_string(i) + "].enable", DirLights[i]->GetEnable());
			myShader->SetInt("lights[" + std::to_string(i) + "].caster", DirLights[i]->GetCaster());
		}
		
		for (unsigned int i = 0; i < PointLights.size(); i++) {
			myShader->SetVec3("lights[" + std::to_string(i + 1) + "].position", PointLights[i]->GetPosition());
			myShader->SetVec3("lights[" + std::to_string(i + 1) + "].ambient", PointLights[i]->GetAmbient());
			myShader->SetVec3("lights[" + std::to_string(i + 1) + "].diffuse", PointLights[i]->GetDiffuse());
			myShader->SetVec3("lights[" + std::to_string(i + 1) + "].specular", PointLights[i]->GetSpecular());
			myShader->SetFloat("lights[" + std::to_string(i + 1) + "].constant", PointLights[i]->GetConstant());
			myShader->SetFloat("lights[" + std::to_string(i + 1) + "].linear", PointLights[i]->GetLinear());
			myShader->SetFloat("lights[" + std::to_string(i + 1) + "].quadratic", PointLights[i]->GetQuadratic());
			myShader->SetFloat("lights[" + std::to_string(i + 1) + "].enable", PointLights[i]->GetEnable());
			myShader->SetInt("lights[" + std::to_string(i + 1) + "].caster", PointLights[i]->GetCaster());
		}

		SpotLights[0]->SetPosition(camera->GetPosition());
		SpotLights[0]->SetDirection(camera->GetFront());
		for (unsigned int i = 0; i < SpotLights.size(); i++) {
			myShader->SetVec3("lights[" + std::to_string(i + 6) + "].position", SpotLights[i]->GetPosition());
			myShader->SetVec3("lights[" + std::to_string(i + 6) + "].direction", SpotLights[i]->GetDirection());
			myShader->SetVec3("lights[" + std::to_string(i + 6) + "].ambient", SpotLights[i]->GetAmbient());
			myShader->SetVec3("lights[" + std::to_string(i + 6) + "].diffuse", SpotLights[i]->GetDiffuse());
			myShader->SetVec3("lights[" + std::to_string(i + 6) + "].specular", SpotLights[i]->GetSpecular());
			myShader->SetFloat("lights[" + std::to_string(i + 6) + "].constant", SpotLights[i]->GetConstant());
			myShader->SetFloat("lights[" + std::to_string(i + 6) + "].linear", SpotLights[i]->GetLinear());
			myShader->SetFloat("lights[" + std::to_string(i + 6) + "].quadratic", SpotLights[i]->GetQuadratic());
			myShader->SetFloat("lights[" + std::to_string(i + 6) + "].cutoff", glm::cos(glm::radians(SpotLights[i]->GetCutoff())));
			myShader->SetFloat("lights[" + std::to_string(i + 6) + "].outerCutoff", glm::cos(glm::radians(SpotLights[i]->GetOuterCutoff())));
			myShader->SetFloat("lights[" + std::to_string(i + 6) + "].exponent", SpotLights[i]->GetExponent());
			myShader->SetBool("lights[" + std::to_string(i + 6) + "].enable", SpotLights[i]->GetEnable());
			myShader->SetInt("lights[" + std::to_string(i + 6) + "].caster", SpotLights[i]->GetCaster());
		}

		
		
		myShader->SetVec4("fog.color", fog->GetColor());
		myShader->SetFloat("fog.density", fog->GetDensity());
		myShader->SetInt("fog.mode", fog->GetMode());
		myShader->SetInt("fog.depthType", fog->GetDepthType());
		myShader->SetBool("fog.enable", fog->GetEnable());
		myShader->SetFloat("fog.f_start", fog->GetFogStart());
		myShader->SetFloat("fog.f_end", fog->GetFogEnd());
		

		// ==================== Draw origin and 3 axes ====================
		if (Settings.ShowOriginAnd3Axes) {
			this->DrawOriginAnd3Axes(myShader.get());
		}

		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(-5.0f, 1.0f, 0.0f)));
			triangle->SetMaterialColor(glm::vec3(0.8f, 0.2f, 0.8f));
			triangle->Draw(myShader.get(), model->Top());
		model->Pop();

		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(4.0f, 1.0f, 0.0f)));
			cube->SetTexture(0, texture_fish.get());
			cube->Draw(myShader.get(), model->Top());
		model->Pop();

		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(8.0f, 1.0f, 0.0f)));
			cube->SetTexture(0, texture_sea.get());
			cube->Draw(myShader.get(), model->Top());
		model->Pop();

		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(12.0f, 1.0f, 0.0f)));
			cube->SetMaterialColor(glm::vec3(0.2f, 0.2f, 0.2f));
			cube->Draw(myShader.get(), model->Top());
		model->Pop();

		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(16.0f, 1.0f, 0.0f)));
			cube->SetTexture(0, texture_sand.get());
			cube->Draw(myShader.get(), model->Top());
		model->Pop();

		// ==================== Draw Skybox (Using Cubemap) ====================



		

		// ==================== Draw Sea ====================
		model->Push();
			floor->SetTexture(0, texture_sea.get());
			floor->SetShininess(64.0f);
			floor->Draw(myShader.get(), model->Top());
		model->Pop();

		
		// ==================== Draw Seabed ====================
		model->Push();
			// ==================== Draw sand ====================
			model->Save(glm::translate(model->Top(), glm::vec3(0.0f, -5.0f, 0.0f)));
			floor->SetTexture(0, texture_sand.get());
			floor->SetShininess(64.0f);
			floor->Draw(myShader.get(), model->Top());

			// ==================== Draw grass ====================
			model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 0.5f, 0.0f)));
			plane->SetTexture(0, texture_grass.get());
			plane->SetShininess(16.0f);
			plane->Draw(myShader.get(), model->Top());

			// ==================== Draw Stone ====================
			model->Push();
				model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 3.0f, 5.0f)));
				cylinder->SetMaterialColor(glm::vec3(0.2, 0.8, 0.9));
				cylinder->SetShininess(16.0f);
				cylinder->Draw(myShader.get(), model->Top());

				model->Save(glm::translate(model->Top(), glm::vec3(3.0f, 0.0f, 0.0f)));
				cylinder->SetMaterialColor(glm::vec3(0.9, 0.0, 0.0));
				cylinder->SetShininess(16.0f);
				cylinder->Draw(myShader.get(), model->Top());
			model->Pop();
		model->Pop();

		// ==================== Draw fishes ====================
		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(0.0f, -2.5f, 0.0f)));
			plane->SetTexture(0, texture_fish.get());
			plane->SetShininess(16.0f);
			plane->Draw(myShader.get(), model->Top());
		model->Pop();

		// ==================== Draw obstacles ====================
		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 0.0f, 0.0f)));
			cube->SetTexture(0, texture_box.get());
			cube->SetTexture(1, texture_box.get());
			cube->SetShininess(64.0f);
			cube->Draw(myShader.get(), model->Top());
		model->Pop();
		
		// ==================== Draw plastic object ====================
		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(3.0f, 0.0f, -3.0f)));
			cube->SetMaterialColor(glm::vec3(0.1f, 0.35f, 0.1f));
			cube->SetShininess(16.0f);
			cube->Draw(myShader.get(), model->Top());
		model->Pop();
		
		// ==================== Draw ROV ====================


		
		// ==================== Draw camera ====================




		

		
		// ==================== draw light ball ====================
		for(unsigned int i = 0; i < PointLights.size(); i++) {
			if (!PointLights[i]->GetEnable()) {
				continue;
			}
			model->Push();
				model->Save(glm::translate(model->Top(), PointLights[i]->GetPosition()));
				model->Save(glm::scale(model->Top(), glm::vec3(0.5f)));
				sphere->SetEnableEmission(true);
				sphere->SetMaterialColor(
					PointLights[i]->GetAmbient(),
					PointLights[i]->GetDiffuse(),
					PointLights[i]->GetSpecular()
				);
				sphere->SetShininess(32.0f);
				sphere->Draw(myShader.get(), model->Top());
				sphere->SetEnableEmission(false);
			model->Pop();
		}

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
			/*
			if (ImGui::BeginTabItem("Projection")) {

				ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), (isPerspective) ? "Perspective Projection" : "Orthogonal Projection");
				ImGui::Text("Parameters");
				ImGui::BulletText("FoV = %.2f deg, Aspect = %.2f", (isGhost) ? camera.Zoom : followCamera.Zoom, aspect_wh);
				ImGui::BulletText("left: %.2f, right: %.2f ", global_left, global_right);
				ImGui::BulletText("bottom: %.2f, top: %.2f ", global_bottom, global_top);
				ImGui::SliderFloat("Near", &global_near, 0.1, 10);
				ImGui::SliderFloat("Far", &global_far, 10, 250);
				ImGui::Spacing();

				if (ImGui::TreeNode("Projection Matrix")) {
					setProjectionMatrix(3);
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

				ImGui::EndTabItem();
			}
			*/


			
			
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

				if (ImGui::TreeNode("ROV Illustration")) {
					ImGui::BulletText("Press W to Forward");
					ImGui::BulletText("Press S to Backward");
					ImGui::BulletText("Press A to Move Left");
					ImGui::BulletText("Press D to Move Right");
					ImGui::BulletText("Press Q to Turn left");
					ImGui::BulletText("Press E to Turn right");
					ImGui::BulletText("Press Left Shift to Dive");
					ImGui::BulletText("Press Space to Rise");
					ImGui::TreePop();
				}
				ImGui::Spacing();

				if (ImGui::TreeNode("Follow Camera Illustration")) {
					ImGui::BulletText("Press O to Increase Distance");
					ImGui::BulletText("Press P to Decrease Distance");
					ImGui::BulletText("Hold mouse right button to change view angle");
					ImGui::BulletText("Mouse scroll to Zoom in / out");
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

				if (ImGui::TreeNode("Illumination Illustration")) {
					ImGui::BulletText("F to turn off/on the spot light");
					ImGui::BulletText("L to switch lighting model");
					ImGui::BulletText("H to switch shading model");
					ImGui::TreePop();
				}
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Illumination")) {

				ImGui::Text("Lighting Model: %s", Settings.UseBlinnPhongShading ? "Blinn-Phong" : "Phong");
				ImGui::Checkbox("use Exponent", &Settings.UseSpotExponent);
				ImGui::Checkbox("Lighting", &Settings.UseLighting);
				ImGui::Checkbox("DiffuseTexture", &Settings.UseDiffuseTexture);
				ImGui::Checkbox("SpecularTexture", &Settings.UseSpecularTexture);
				ImGui::Checkbox("Emission", &Settings.UseEmission);
				ImGui::Checkbox("Gamma Correction", &Settings.UseGamma);
				ImGui::SliderFloat("Gamma Value", &Settings.GammaValue, 1.0f / 2.2f, 2.2f);
				ImGui::Spacing();

				for (unsigned int i = 0; i < DirLights.size(); i++) {
					if (ImGui::TreeNode(std::string("Directional Light " + std::to_string(i)).c_str())) {
						DirLights[i]->GenerateDebugUI();
						ImGui::TreePop();
					}
					ImGui::Spacing();
				}

				for (unsigned int i = 0; i < PointLights.size(); i++) {
					if (ImGui::TreeNode(std::string("Point Light " + std::to_string(i)).c_str())) {
						PointLights[i]->GenerateDebugUI();
						ImGui::TreePop();
					}
					ImGui::Spacing();
				}

				for (unsigned int i = 0; i < SpotLights.size(); i++) {
					if (ImGui::TreeNode(std::string("Spot Light " + std::to_string(i)).c_str())) {
						SpotLights[i]->GenerateDebugUI();
						ImGui::TreePop();
					}
					ImGui::Spacing();
				}

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
			if (ImGui::BeginTabItem("Fog")) {
				fog->GenerateDebugUI();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::End();
	}

	void DrawOriginAnd3Axes(Nexus::Shader* shader) const {
		// 繪製世界坐標系原點（0, 0, 0）
		model->Push();
			model->Save(glm::scale(model->Top(), glm::vec3(0.2f, 0.2f, 0.2f)));
			sphere->SetMaterialColor(glm::vec3(0.2f, 0.2f, 0.2f));
			sphere->Draw(shader, model->Top());
		model->Pop();

		// 繪製三個軸
		model->Push();
			model->Push();
				model->Save(glm::translate(model->Top(), glm::vec3(1.5f, 0.0f, 0.0f)));
				model->Save(glm::scale(model->Top(), glm::vec3(3.0f, 0.1f, 0.1f)));
				cube->SetMaterialColor(glm::vec3(1.0f, 0.0f, 0.0f));
				cube->Draw(shader, model->Top());
			model->Pop();

			model->Push();
				model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 1.5f, 0.0f)));
				model->Save(glm::scale(model->Top(), glm::vec3(0.1f, 3.0f, 0.1f)));
				cube->SetMaterialColor(glm::vec3(0.0f, 1.0f, 0.0f));
				cube->Draw(shader, model->Top());
			model->Pop();

			model->Push();
				model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 0.0f, 1.5f)));
				model->Save(glm::scale(model->Top(), glm::vec3(0.1f, 0.1f, 3.0f)));
				cube->SetMaterialColor(glm::vec3(0.0f, 0.0f, 1.0f));
				cube->Draw(shader, model->Top());
			model->Pop();
		model->Pop();
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

		if (key == GLFW_KEY_X) {
			if (Settings.ShowOriginAnd3Axes) {
				Settings.ShowOriginAnd3Axes = false;
				Nexus::Logger::Message(Nexus::LOG_INFO, "World coordinate origin and 3 axes: [Hide].");
			} else {
				Settings.ShowOriginAnd3Axes = true;
				Nexus::Logger::Message(Nexus::LOG_INFO, "World coordinate origin and 3 axes: [Show].");
			}
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
	
	std::unique_ptr<Nexus::MatrixStack> model = nullptr;
	std::unique_ptr<Nexus::MatrixStack> view = nullptr;
	std::unique_ptr<Nexus::MatrixStack> projection = nullptr;

	std::unique_ptr<Nexus::Triangle> triangle = nullptr;
	std::unique_ptr<Nexus::Rectangle> floor = nullptr;
	std::unique_ptr<Nexus::Rectangle> plane = nullptr;
	std::unique_ptr<Nexus::Rectangle> square = nullptr;
	std::unique_ptr<Nexus::Cube> cube = nullptr;
	std::unique_ptr<Nexus::Sphere> sphere = nullptr;
	std::unique_ptr<Nexus::Cylinder> cylinder = nullptr;

	std::unique_ptr<Nexus::Texture2D> texture_sea = nullptr;
	std::unique_ptr<Nexus::Texture2D> texture_sand = nullptr;
	std::unique_ptr<Nexus::Texture2D> texture_grass = nullptr;
	std::unique_ptr<Nexus::Texture2D> texture_box = nullptr;
	std::unique_ptr<Nexus::Texture2D> texture_box_spec = nullptr;
	std::unique_ptr<Nexus::Texture2D> texture_fish = nullptr;

	std::vector<Nexus::DirectionalLight*> DirLights;
	std::vector<Nexus::PointLight*> PointLights;
	std::vector<Nexus::SpotLight*> SpotLights;

	std::unique_ptr<Nexus::Fog> fog;
};

int main() {
	NexusDemo app;
	return app.Run();
}