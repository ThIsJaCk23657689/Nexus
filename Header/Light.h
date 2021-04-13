#pragma once

#include <glm/glm.hpp>

namespace Nexus {

	enum LightCaster {
		LIGHT_DIRECTION,
		LIGHT_POINT,
		LIGHT_SPOT
	};

	const glm::vec3 AMBIENT = glm::vec3(0.1f);
	const glm::vec3 DIFFUSE = glm::vec3(0.7f);
	const glm::vec3 SPECULAR = glm::vec3(0.4f);

	const float LINEAR = 0.09f;
	const float QUADRATIC = 0.0032f;

	const float CUTOFF = 12.0f;
	const float OUTERCUTOFF = 15.0f;

	
	class Light {
	public:
		Light() : Ambient(AMBIENT), Diffuse(DIFFUSE), Specular(SPECULAR) {
		}

		~Light()  = default;

		glm::vec3 GetAmbient() const { return this->Ambient; }
		glm::vec3 GetDiffuse() const { return this->Diffuse; }
		glm::vec3 GetSpecular() const { return this->Specular; }
		bool GetEnable() const { return this->Enable; }
		unsigned int GetCaster() const { return this->Caster; }

		void SetAmbient(glm::vec3 ambient) {
			this->Ambient = ambient;
		}

		void SetDiffuse(glm::vec3 diffuse) {
			this->Diffuse = diffuse;
		}

		void SetSpecular(glm::vec3 specular) {
			this->Specular = specular;
		}

		void SetEnable(bool enable) {
			this->Enable = enable;
		}
		
	protected:
		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;

		bool Enable;
		unsigned int Caster;
	};

	class DirectionalLight : public Light {
	public:
		DirectionalLight(glm::vec3 direction = glm::vec3(0.0f, 0.1f, 0.1f), bool enable = true) {
			this->Caster = LIGHT_DIRECTION;
			this->Direction = direction;
			this->Enable = enable;
		}

		~DirectionalLight() = default;

		glm::vec3 GetDirection() const { return this->Direction; }

		void SetDirection(glm::vec3 direction) {
			this->Direction = direction;
		}

		void GenerateDebugUI() {
			ImGui::Text(std::string("Direction: (%.2f, %.2f, %.2f)").c_str(), this->Direction.x, this->Direction.y, this->Direction.z);
			ImGui::SliderFloat3(std::string("Ambient").c_str(), (float*)&this->Ambient, 0.0f, 1.0f);
			ImGui::SliderFloat3(std::string("Diffuse").c_str(), (float*)&this->Diffuse, 0.0f, 1.0f);
			ImGui::SliderFloat3(std::string("Specular").c_str(), (float*)&this->Specular, 0.0f, 1.0f);
			ImGui::Checkbox(std::string("Enable").c_str(), &this->Enable);
			ImGui::Spacing();
		}
		
	protected:
		glm::vec3 Direction;
		
	};

	class PointLight : public Light {
	public:
		PointLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), bool enable = true) : Constant(1.0f), Linear(LINEAR), Quadratic(QUADRATIC) {
			this->Caster = LIGHT_POINT;
			this->Position = position;
			this->Enable = enable;
		}
		
		~PointLight() = default;
		
		glm::vec3 GetPosition() const { return this->Position; }
		float GetConstant() const { return this->Constant; }
		float GetLinear() const { return this->Linear; }
		float GetQuadratic() const { return this->Quadratic; }

		void SetPosition(glm::vec3 position) {
			this->Position = position;
		}

		void SetConstant(float value) {
			this->Constant = value;
		}

		void SetLinear(float value) {
			this->Linear = value;
		}

		void SetQuadratic(float value) {
			this->Quadratic = value;
		}

		void GenerateDebugUI() {
			/*
			if (i != 4) {
				// ROV的光，因為位置是鎖定在ROV上，所以這邊不給調整
				ImGui::SliderFloat3(std::string("Position").c_str(), (float*)&PointLights[i]->GetPosition(), -50.0f, 50.0f);
			}
			*/
			ImGui::SliderFloat3(std::string("Position").c_str(), (float*)&this->Position, -50.0f, 50.0f);
			ImGui::SliderFloat3(std::string("Ambient").c_str(), (float*)&this->Ambient, 0.0f, 1.0f);
			ImGui::SliderFloat3(std::string("Diffuse").c_str(), (float*)&this->Diffuse, 0.0f, 1.0f);
			ImGui::SliderFloat3(std::string("Specular").c_str(), (float*)&this->Specular, 0.0f, 1.0f);
			
			/*
			if (i != 4) {
				// ROV的Specular，不給調整，看起來才不會怪怪的
				ImGui::SliderFloat3(std::string("Specular").c_str(), (float*)&PointLights[i]->GetSpecular(), 0.0f, 1.0f);
			}
			*/

			ImGui::SliderFloat(std::string("Linear").c_str(), (float*)&this->Linear, 0.00014f, 0.7f);
			ImGui::SliderFloat(std::string("Quadratic").c_str(), (float*)&this->Quadratic, 0.00007, 0.5f);
			ImGui::Checkbox(std::string("Enable").c_str(), &this->Enable);
			ImGui::Spacing();
		}
		
	protected:
		glm::vec3 Position;

		float Constant;
		float Linear;
		float Quadratic;
	};

	class SpotLight : public Light {
	public:
		SpotLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f), bool enable = true) : Constant(1.0f), Linear(LINEAR), Quadratic(QUADRATIC), Cutoff(CUTOFF), OuterCutoff(OUTERCUTOFF) {
			this->Caster = LIGHT_SPOT;
			this->Position = position;
			this->Direction = direction;
			this->Enable = enable;
		}

		~SpotLight() = default;

		glm::vec3 GetDirection() const { return this->Direction; }
		glm::vec3 GetPosition() const { return this->Position; }
		float GetConstant() const { return this->Constant; }
		float GetLinear() const { return this->Linear; }
		float GetQuadratic() const { return this->Quadratic; }
		float GetCutoff() const { return this->Cutoff; }
		float GetOuterCutoff() const { return this->OuterCutoff; }

		void SetDirection(glm::vec3 direction) {
			this->Direction = direction;
		}
		
		void SetPosition(glm::vec3 position) {
			this->Position = position;
		}

		void SetConstant(float value) {
			this->Constant = value;
		}

		void SetLinear(float value) {
			this->Linear = value;
		}

		void SetQuadratic(float value) {
			this->Quadratic = value;
		}

		void SetCutoff(float value) {
			this->Cutoff = value;
		}

		void SetOuterCutoff(float value) {
			this->OuterCutoff = value;
		}

		void GenerateDebugUI() {
			ImGui::Text(std::string("Position: (%.2f, %.2f, %.2f)").c_str(), this->Position.x, this->Position.y, this->Position.z);
			ImGui::Text(std::string("Direction: (%.2f, %.2f, %.2f)").c_str(), this->Direction.x, this->Direction.y, this->Direction.z);
			ImGui::SliderFloat3(std::string("Ambient").c_str(), (float*)&this->Ambient, 0.0f, 1.0f);
			ImGui::SliderFloat3(std::string("Diffuse").c_str(), (float*)&this->Diffuse, 0.0f, 1.0f);
			ImGui::SliderFloat3(std::string("Specular").c_str(), (float*)&this->Specular, 0.0f, 1.0f);
			ImGui::SliderFloat(std::string("Linear").c_str(), (float*)&this->Linear, 0.00014f, 0.7f);
			ImGui::SliderFloat(std::string("Quadratic").c_str(), (float*)&this->Quadratic, 0.00007, 0.5f);
			ImGui::SliderFloat(std::string("Cutoff").c_str(), (float*)&this->Cutoff, 0.0f, this->OuterCutoff - 1);
			ImGui::SliderFloat(std::string("OuterCutoff").c_str(), (float*)&this->OuterCutoff, this->Cutoff + 1, 40.0f);
			ImGui::Checkbox(std::string("Enable").c_str(), &this->Enable);
			ImGui::Spacing();
		}
		
	protected:
		glm::vec3 Position;
		glm::vec3 Direction;

		float Constant;
		float Linear;
		float Quadratic;

		float Cutoff;
		float OuterCutoff;
	};
}