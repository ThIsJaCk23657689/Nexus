#include <imgui.h>

#include "Application.h"
#include "Logger.h"
#include "FirsrPersonCamera.h"
#include "ThirdPersonCamera.h"
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

class NexusDemo final : public Nexus::Application {
public:
	NexusDemo() {
		Settings.Width = 800;
		Settings.Height = 600;
		Settings.WindowTitle = "NexusDemo | Nexus";
		Settings.EnableDebugCallback = true;
		Settings.EnableFullScreen = false;

		Settings.EnableGhostMode = true;
		Settings.ShowOriginAnd3Axes = false;
		
		Settings.UseBlinnPhongShading = false;
		Settings.UseSpotExponent = false;
		Settings.UseLighting = true;
		Settings.UseDiffuseTexture = true;
		Settings.UseSpecularTexture = true;
		Settings.UseEmission = true;
		Settings.UseGamma = false;
		Settings.GammaValue = 1.0f / 2.2f;

		// Projection Settings Initalize
		ProjectionSettings.IsPerspective = true;
		ProjectionSettings.ClippingNear = 0.1f;
		ProjectionSettings.ClippingFar = 500.0f;
		ProjectionSettings.Aspect = (float)Settings.Width / (float)Settings.Height;
	}

	void Initialize() override {
		
		// Setting OpenGL
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Create shader program
		// myShader = std::make_unique<Nexus::Shader>("Shaders/testing.vert", "Shaders/testing.frag");
		myShader = std::make_unique<Nexus::Shader>("Shaders/lighting.vert", "Shaders/lighting.frag");
		normalShader = std::make_unique<Nexus::Shader>("Shaders/normal_visualization.vs", "Shaders/normal_visualization.fs", "Shaders/normal_visualization.gs");
		
		// Create Camera
		first_camera = std::make_unique<Nexus::FirstPersonCamera>(glm::vec3(0.0f, 0.0f, 5.0f));
		third_camera = std::make_unique<Nexus::ThirdPersonCamera>(glm::vec3(0.0f, 0.0f, 5.0f));

		// Create Matrix Stack
		model = std::make_unique<Nexus::MatrixStack>();

		// Create object data
		triangle = std::make_unique<Nexus::Triangle>();
		floor = std::make_unique<Nexus::Rectangle>(200.0f, 200.0f, 25.0f, Nexus::POS_Y);
		plane = std::make_unique<Nexus::Rectangle>();
		square = std::make_unique<Nexus::Rectangle>(Nexus::POS_X);
		cube = std::make_unique<Nexus::Cube>();;
		sphere = std::make_unique<Nexus::Sphere>();
		cylinder = std::make_unique<Nexus::Cylinder>(1.0f, 0.1f, 3.0f);

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
			new Nexus::PointLight(glm::vec3(10.0f, 10.0f, 35.0f), false),
			new Nexus::PointLight(glm::vec3(-45.0f, 5.0f, 30.0f), false),
			new Nexus::PointLight(glm::vec3(38.0f, 2.0f, -40.0f), false),
			new Nexus::PointLight(glm::vec3(-50.0f, 15.0f, -45.0f), false),
			new Nexus::PointLight(glm::vec3(0.0f, 10.0f, 0.0f), false)
		};
		SpotLights = {
			new Nexus::SpotLight(third_camera->GetPosition(), third_camera->GetFront(), false),
			new Nexus::SpotLight(first_camera->GetPosition(), first_camera->GetFront(), false)
		};

		// Fog
		fog = std::make_unique<Nexus::Fog>(glm::vec4(0.266f, 0.5f, 0.609f, 1.0f), false, 0.1f, 100.0f);
	}
	
	void Update() override {

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SetViewMatrix(Nexus::DISPLAY_MODE_DEFAULT);
		SetProjectionMatrix(Nexus::DISPLAY_MODE_DEFAULT);

		myShader->Use();
		myShader->SetInt("material.diffuse_texture", 0);
		myShader->SetInt("material.specular_texture", 1);
		myShader->SetInt("material.emission_texture", 2);
		myShader->SetInt("skybox", 3);

		myShader->SetMat4("view", view);
		myShader->SetMat4("projection", projection);
		myShader->SetVec3("viewPos", Settings.EnableGhostMode ? first_camera->GetPosition() : third_camera->GetPosition());

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

		SpotLights[0]->SetPosition(third_camera->GetPosition());
		SpotLights[0]->SetDirection(third_camera->GetFront());
		SpotLights[1]->SetPosition(first_camera->GetPosition());
		SpotLights[1]->SetDirection(first_camera->GetFront());
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


		
		/*
		normalShader->Use();
		normalShader->SetMat4("view", view);
		normalShader->SetMat4("projection", projection);
		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(8.0f, 1.0f, 0.0f)));
			cube->SetTexture(0, texture_sea.get());
			cube->Draw(normalShader.get(), model->Top());
		model->Pop();
		*/
		

		// ==================== Draw origin and 3 axes ====================
		if (Settings.ShowOriginAnd3Axes) {
			this->DrawOriginAnd3Axes(myShader.get());
		}

		myShader->SetBool("material.enableDiffuseTexture", false);
		myShader->SetBool("material.enableSpecularTexture", false);
		myShader->SetBool("material.enableEmission", false);
		myShader->SetBool("material.enableEmissionTexture", false);
		myShader->SetVec4("material.ambient", glm::vec4(0.6f, 0.042f, 0.85f, 1.0));
		myShader->SetVec4("material.diffuse", glm::vec4(0.6f, 0.042f, 0.85f, 1.0));
		myShader->SetVec4("material.specular", glm::vec4(0.6f, 0.042f, 0.85f, 1.0));
		myShader->SetFloat("material.shininess", 64.0f);
		model->Push();
		floor->Draw(myShader.get(), model->Top());
		model->Pop();
		
		// ==================== Draw Skybox (Using Cubemap) ====================



		
		/*
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
		
		myShader->SetBool("material.enableDiffuseTexture", true);
		myShader->SetBool("material.enableSpecularTexture", true);
		myShader->SetBool("material.enableEmission", false);
		myShader->SetBool("material.enableEmissionTexture", false);
		myShader->SetFloat("material.shininess", 64.0f);
		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 0.0f, 0.0f)));
			cube->Draw(myShader.get(), model->Top());
		model->Pop();
		*/
		

		
		
		// ==================== Draw plastic object ====================
		myShader->SetBool("material.enableDiffuseTexture", false);
		myShader->SetBool("material.enableSpecularTexture", false);
		myShader->SetBool("material.enableEmission", false);
		myShader->SetBool("material.enableEmissionTexture", false);
		myShader->SetVec4("material.ambient", glm::vec4(0.02f, 0.02f, 0.02f, 1.0));
		myShader->SetVec4("material.diffuse", glm::vec4(0.1f, 0.35f, 0.1f, 1.0));
		myShader->SetVec4("material.specular", glm::vec4(0.45f, 0.55f, 0.45f, 1.0));
		myShader->SetFloat("material.shininess", 16.0f);
		model->Push();
			model->Save(glm::translate(model->Top(), glm::vec3(3.0f, 0.0f, -3.0f)));
			cube->Draw(myShader.get(), model->Top());
		model->Pop();
		
		// ==================== Draw ROV ====================


		
		// ==================== Draw camera ====================




		

		
		// ==================== draw light ball ====================
		myShader->SetBool("material.enableDiffuseTexture", false);
		myShader->SetBool("material.enableSpecularTexture", false);
		myShader->SetBool("material.enableEmission", true);
		myShader->SetBool("material.enableEmissionTexture", false);
		for(unsigned int i = 0; i < PointLights.size(); i++) {
			if (!PointLights[i]->GetEnable()) {
				continue;
			}
			model->Push();
				model->Save(glm::translate(model->Top(), PointLights[i]->GetPosition()));
				model->Save(glm::scale(model->Top(), glm::vec3(0.5f)));
				myShader->SetVec4("material.ambient", glm::vec4(PointLights[i]->GetAmbient(), 1.0f));
				myShader->SetVec4("material.diffuse", glm::vec4(PointLights[i]->GetDiffuse(), 1.0f));
				myShader->SetVec4("material.specular", glm::vec4(PointLights[i]->GetSpecular(), 1.0f));
				myShader->SetFloat("material.shininess", 32.0f);
				sphere->Draw(myShader.get(), model->Top());
			model->Pop();
		}
		myShader->SetBool("material.enableEmission", false);

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
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				if (Settings.EnableGhostMode) {
					first_camera->ShowDebugUI("Person Person Camera");
				} else {
					third_camera->ShowDebugUI("Third Person Camera");
					ImGui::BulletText("Distance: %.2f", third_camera->GetDistance());
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Projection")) {

				ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), (ProjectionSettings.IsPerspective) ? "Perspective Projection" : "Orthogonal Projection");
				ImGui::Text("Parameters");
				ImGui::BulletText("FoV = %.2f deg, Aspect = %.2f", Settings.EnableGhostMode ? first_camera->GetFOV() : third_camera->GetFOV(), ProjectionSettings.Aspect);
				if (!ProjectionSettings.IsPerspective) {
					ImGui::SliderFloat("Length", &ProjectionSettings.OrthogonalHeight, 10.0f, 200.0f);
				}
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
		shader->SetBool("material.enableDiffuseTexture", false);
		shader->SetBool("material.enableSpecularTexture", false);
		shader->SetBool("material.enableEmission", true);
		shader->SetBool("material.enableEmissionTexture", false);
		
		// 繪製世界坐標系原點（0, 0, 0）
		model->Push();
		model->Save(glm::scale(model->Top(), glm::vec3(0.4f, 0.4f, 0.4f)));
		shader->SetVec4("material.ambient", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		shader->SetVec4("material.diffuse", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		shader->SetVec4("material.specular", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
		shader->SetFloat("material.shininess", 64.0f);
		sphere->Draw(shader, model->Top());
		model->Pop();

		// 繪製三個軸
		model->Push();
		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(100.0f, 0.0f, 0.0f)));
		model->Save(glm::scale(model->Top(), glm::vec3(200.0f, 1.0f, 1.0f)));
		shader->SetVec4("material.ambient", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		shader->SetVec4("material.diffuse", glm::vec4(1.0f, 0.0f, 0.0f, 1.0));
		shader->SetVec4("material.specular", glm::vec4(1.0f, 0.0f, 0.0f, 1.0));
		shader->SetFloat("material.shininess", 64.0f);
		cube->Draw(shader, model->Top());
		model->Pop();

		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 100.0f, 0.0f)));
		model->Save(glm::scale(model->Top(), glm::vec3(1.0f, 200.0f, 1.0f)));
		shader->SetVec4("material.ambient", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		shader->SetVec4("material.diffuse", glm::vec4(0.0f, 1.0f, 0.0f, 1.0));
		shader->SetVec4("material.specular", glm::vec4(0.0f, 1.0f, 0.0f, 1.0));
		shader->SetFloat("material.shininess", 64.0f);
		cube->Draw(shader, model->Top());
		model->Pop();

		model->Push();
		model->Save(glm::translate(model->Top(), glm::vec3(0.0f, 0.0f, 100.0f)));
		model->Save(glm::scale(model->Top(), glm::vec3(1.0f, 1.0f, 200.0f)));
		shader->SetVec4("material.ambient", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		shader->SetVec4("material.diffuse", glm::vec4(0.0f, 0.0f, 1.0f, 1.0));
		shader->SetVec4("material.specular", glm::vec4(0.0f, 0.0f, 1.0f, 1.0));
		shader->SetFloat("material.shininess", 64.0f);
		cube->Draw(shader, model->Top());
		model->Pop();
		model->Pop();
	}

	void SetViewMatrix(Nexus::DisplayMode monitor_type) {
		glm::vec3 camera_position = Settings.EnableGhostMode ? first_camera->GetPosition() : third_camera->GetPosition();
		switch (monitor_type) {
			case Nexus::DISPLAY_MODE_ORTHOGONAL_X:
				view = glm::lookAt(camera_position + glm::vec3(5.0, 0.0, 0.0), camera_position, glm::vec3(0.0, 1.0, 0.0));
				break;
			case Nexus::DISPLAY_MODE_ORTHOGONAL_Y:
				view = glm::lookAt(camera_position + glm::vec3(0.0, 5.0, 0.0), camera_position, glm::vec3(0.0, 0.0, -1.0));
				break;
			case Nexus::DISPLAY_MODE_ORTHOGONAL_Z:
				view = glm::lookAt(camera_position + glm::vec3(0.0, 0.0, 5.0), camera_position, glm::vec3(0.0, 1.0, 0.0));
				break;
			case Nexus::DISPLAY_MODE_DEFAULT:
				view = Settings.EnableGhostMode ? first_camera->GetViewMatrix() : third_camera->GetViewMatrix();
				break;
		}
	}

	void SetProjectionMatrix(Nexus::DisplayMode monitor_type) {
		ProjectionSettings.Aspect = (float)Settings.Width / (float)Settings.Height;

		if (monitor_type == Nexus::DISPLAY_MODE_DEFAULT) {
			if (ProjectionSettings.IsPerspective) {
				projection = GetPerspectiveProjMatrix(glm::radians(Settings.EnableGhostMode ? first_camera->GetFOV() : third_camera->GetFOV()), ProjectionSettings.Aspect, ProjectionSettings.ClippingNear, ProjectionSettings.ClippingFar);
			} else {
				projection = GetOrthoProjMatrix(-ProjectionSettings.OrthogonalHeight * ProjectionSettings.Aspect, ProjectionSettings.OrthogonalHeight * ProjectionSettings.Aspect, -ProjectionSettings.OrthogonalHeight, ProjectionSettings.OrthogonalHeight, ProjectionSettings.ClippingNear, ProjectionSettings.ClippingFar);
			}
		} else {
			projection = GetOrthoProjMatrix(-5.0 * ProjectionSettings.Aspect, 5.0 * ProjectionSettings.Aspect, -5.0, 5.0, 0.1f, 250.0f);
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
		} else {
			glViewport(0, 0, Settings.Width, Settings.Height);
		}
	}
	
	void OnWindowResize() override {
		ProjectionSettings.Aspect = (float)Settings.Width / (float)Settings.Height;

		// Reset viewport
		SetViewport(Nexus::DISPLAY_MODE_DEFAULT);
	}
	
	void OnProcessInput(int key) override {
		if (Settings.EnableGhostMode) {
			if (key == GLFW_KEY_W) {
				first_camera->ProcessKeyboard(Nexus::CAMERA_FORWARD, DeltaTime);
			}
			if (key == GLFW_KEY_S) {
				first_camera->ProcessKeyboard(Nexus::CAMERA_BACKWARD, DeltaTime);
			}
			if (key == GLFW_KEY_A) {
				first_camera->ProcessKeyboard(Nexus::CAMERA_LEFT, DeltaTime);
			}
			if (key == GLFW_KEY_D) {
				first_camera->ProcessKeyboard(Nexus::CAMERA_RIGHT, DeltaTime);
			}
		}
	}
	
	void OnKeyPress(int key) override {
		if (key == GLFW_KEY_LEFT_SHIFT) {
			if (Settings.EnableGhostMode) {
				first_camera->SetMovementSpeed(500.0f);
			}
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

		if (key == GLFW_KEY_G) {
			if (Settings.EnableGhostMode) {
				Settings.EnableGhostMode = false;
				SpotLights[1]->SetEnable(false);
				Nexus::Logger::Message(Nexus::LOG_INFO, "Camera Mode: Third Person");
			} else {
				Settings.EnableGhostMode = true;
				SpotLights[1]->SetEnable(true);
				Nexus::Logger::Message(Nexus::LOG_INFO, "Camera Mode: First Person");
			}
		}

		// 手電筒開關
		if (key == GLFW_KEY_F) {
			if (Settings.EnableGhostMode) {
				if (SpotLights[1]->GetEnable()) {
					SpotLights[1]->SetEnable(false);
					Nexus::Logger::Message(Nexus::LOG_INFO, "Spot Light 1 is turn off.");
				} else {
					SpotLights[1]->SetEnable(true);
					Nexus::Logger::Message(Nexus::LOG_INFO, "Spot Light 1 is turn on.");
				}
			} else {
				if (SpotLights[0]->GetEnable()) {
					SpotLights[0]->SetEnable(false);
					Nexus::Logger::Message(Nexus::LOG_INFO, "Spot Light 0 is turn off.");
				}
				else {
					SpotLights[0]->SetEnable(true);
					Nexus::Logger::Message(Nexus::LOG_INFO, "Spot Light 0 is turn on.");
				}
			}
		}
	}
	
	void OnKeyRelease(int key) override {
		if (key == GLFW_KEY_LEFT_SHIFT) {
			if (Settings.EnableGhostMode) {
				first_camera->SetMovementSpeed(50.0f);
			}
		}
	}
	
	void OnMouseMove(int xoffset, int yoffset) override {
		if (!Settings.EnableCursor) {
			if (Settings.EnableGhostMode) {
				first_camera->ProcessMouseMovement(xoffset, yoffset);
			} else {
				third_camera->ProcessMouseMovement(xoffset, yoffset);
			}
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
		if (ProjectionSettings.IsPerspective) {
			if (Settings.EnableGhostMode) {
				first_camera->ProcessMouseScroll(yoffset);
			}
			else {
				third_camera->AdjustDistance(yoffset);
			}
		} else {
			AdjustOrthogonalProjectionWidth(yoffset);
		}
	}

	void AdjustOrthogonalProjectionWidth(float yoffset) {
		if (ProjectionSettings.OrthogonalHeight >= 10.0f && ProjectionSettings.OrthogonalHeight <= 200.0f) {
			ProjectionSettings.OrthogonalHeight -= (float)yoffset * 10.0f;
		}
		if (ProjectionSettings.OrthogonalHeight < 10.0f) {
			ProjectionSettings.OrthogonalHeight = 10.0f;
		}
		if (ProjectionSettings.OrthogonalHeight > 200.0f) {
			ProjectionSettings.OrthogonalHeight = 200.0f;
		}
	}

private:
	std::unique_ptr<Nexus::Shader> myShader = nullptr;
	std::unique_ptr<Nexus::Shader> normalShader = nullptr;
	
	std::unique_ptr<Nexus::FirstPersonCamera> first_camera = nullptr;
	std::unique_ptr<Nexus::ThirdPersonCamera> third_camera = nullptr;
	
	std::unique_ptr<Nexus::MatrixStack> model = nullptr;
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

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