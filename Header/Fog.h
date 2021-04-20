#pragma once

#include <glm/glm.hpp>

namespace Nexus {
	
	enum FogMode {
		FOG_MODE_LINEAR,
		FOG_MODE_EXP,
		FOG_MODE_EXP2
	};

	enum FogDepthType {
		FOG_PLANE_BASED,
		FOG_RANGE_BASED
	};

	const float DENSITY = 0.15f;
	
	class Fog {
	public:
		Fog(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), bool enable = true, float f_start = 20.0f, float f_end = 80.0f) : Density(DENSITY) {
			this->Mode = FOG_MODE_EXP;
			this->DepthType = FOG_RANGE_BASED;
			this->FogStart = f_start;
			this->FogEnd = f_end;
			this->Enable = enable;
			this->Color = color;
		}

		~Fog() = default;

		unsigned int GetMode() const {
			return this->Mode;
		}

		unsigned int GetDepthType() const {
			return this->DepthType;
		}

		float GetDensity() const {
			return this->Density;
		}

		float GetFogStart() const {
			return this->FogStart;
		}

		float GetFogEnd() const {
			return this->FogEnd;
		}

		bool GetEnable() const {
			return this->Enable;
		}

		glm::vec4 GetColor() const {
			return this->Color;
		}

		void SetDensity(float density) {
			this->Density = density;
		}

		void GenerateDebugUI() {
			// ImGui::Checkbox(std::string("Manual Control").c_str(), &fogManual);
			ImGui::SliderFloat4(std::string("Color").c_str(), (float*)&this->Color, 0.0f, 1.0f);
			ImGui::SliderFloat(std::string("Density").c_str(), (float*)&this->Density, 0.0f, 1.0f);

			const char* items_a[] = { "LINEAR", "EXP", "EXP2" };
			const char* items_b[] = { "PLANE_BASED", "RANGE_BASED" };
			ImGui::Combo("Mode", (int*)&this->Mode, items_a, IM_ARRAYSIZE(items_a));
			ImGui::Combo("Depth Type", (int*)&this->DepthType, items_b, IM_ARRAYSIZE(items_b));

			if (this->Mode == FOG_MODE_LINEAR) {
				ImGui::SliderFloat(std::string("Fog Start").c_str(), &this->FogStart, 0.1f, this->FogEnd);
				ImGui::SliderFloat(std::string("Fog End").c_str(), &this->FogEnd, this->FogStart, 100.0f);
			}

			ImGui::Checkbox(std::string("Enable").c_str(), &this->Enable);
			ImGui::Spacing();
		}

	private:
		unsigned int Mode;
		unsigned int DepthType;
		float Density;
		float FogStart;
		float FogEnd;
		bool Enable;
		glm::vec4 Color;
	};
}