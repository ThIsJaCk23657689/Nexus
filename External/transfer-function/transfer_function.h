#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

class TransferFunction {
public:
	std::vector<float> points = { 0.950f, 0.050f, 0.795f, 0.035f };
	
	TransferFunction() = default;

	void DrawTransferFunction(const std::string& label, float* point) {
		// Visual Parameters Setting
		enum { CURVE_WIDTH = 4 };
		enum { LINE_WIDTH = 1 };
		enum { GRAB_RADIUS = 8 };
		enum { GRAB_BORDER = 2 };
		enum { AREA_CONSTRAINED = true };
		enum { AREA_WIDTH = 128 };

		// Get ImGui Parameters
		const ImGuiStyle& Style = ImGui::GetStyle();
		const ImGuiIO& IO = ImGui::GetIO();
		ImDrawList* DrawList = ImGui::GetWindowDrawList();
		ImGuiWindow* Window = ImGui::GetCurrentWindow();
		if (Window->SkipItems) return;

		// Spacing
		ImGui::Text("Points: (%.3f, %.3f, %.3f, %.3f)", points[0], points[1], points[2], points[3]);
		int hovered = ImGui::IsItemActive() || ImGui::IsItemHovered();
		ImGui::Dummy(ImVec2(0, 3));

		// Prepare Canvas
		const float avail = ImGui::GetContentRegionAvailWidth();
		const float dim = AREA_WIDTH > 0 ? AREA_WIDTH : avail;
		ImVec2 Canvas(dim, dim);

		ImRect bb(Window->DC.CursorPos, Window->DC.CursorPos + Canvas);
		ImGui::ItemSize(bb);

		const ImGuiID id = Window->GetID(label.c_str());
		hovered |= 0 != ImGui::ItemHoverable(ImRect(bb.Min, bb.Min + ImVec2(avail, dim)), id);

		ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);

		// background grid
		for (int i = 0; i <= Canvas.x; i += (Canvas.x / 4)) {
			DrawList->AddLine(
				ImVec2(bb.Min.x + i, bb.Min.y),
				ImVec2(bb.Min.x + i, bb.Max.y),
				ImGui::GetColorU32(ImGuiCol_TextDisabled));
		}
		for (int i = 0; i <= Canvas.y; i += (Canvas.y / 4)) {
			DrawList->AddLine(
				ImVec2(bb.Min.x, bb.Min.y + i),
				ImVec2(bb.Max.x, bb.Min.y + i),
				ImGui::GetColorU32(ImGuiCol_TextDisabled));
		}

		ImVec2 mouse = ImGui::GetIO().MousePos, pos[2];
		float distance[2];

		for (int i = 0; i < 2; i++) {
			pos[i] = ImVec2(points[i * 2 + 0], 1 - points[i * 2 + 1]) * (bb.Max - bb.Min) + bb.Min;
			distance[i] = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) + (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
		}

		int selected = distance[0] < distance[1] ? 0 : 1;
		if (distance[selected] < (4 * GRAB_RADIUS * 4 * GRAB_RADIUS)) {
			ImGui::SetTooltip("(%4.3f, %4.3f)", points[selected * 2 + 0], points[selected * 2 + 1]);
			if (ImGui::IsMouseClicked(0) || ImGui::IsMouseDragging(0)) {
				float& px = (points[selected * 2 + 0] += ImGui::GetIO().MouseDelta.x / Canvas.x);
				float& py = (points[selected * 2 + 1] -= ImGui::GetIO().MouseDelta.y / Canvas.y);

				if (AREA_CONSTRAINED) {
					px = (px < 0 ? 0 : (px > 1 ? 1 : px));
					py = (py < 0 ? 0 : (py > 1 ? 1 : py));
				}
			}
		}

		// Draw line
		/*
		ImColor color(ImGui::GetStyle().Colors[ImGuiCol_PlotLines]);
		for (int i = 0; i < SMOOTHNESS; ++i) {
			ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
			ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
			ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
			ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
			DrawList->AddLine(r, s, color, CURVE_WIDTH);
		}
		*/

		// draw lines and grabbers
		ImVec4 white(ImGui::GetStyle().Colors[ImGuiCol_Text]);
		float luma = ImGui::IsItemActive() || ImGui::IsItemHovered() ? 0.5f : 1.0f;
		ImVec4 pink(1.00f, 0.00f, 0.75f, luma), cyan(0.00f, 0.75f, 1.00f, luma);
		ImVec2 p1 = ImVec2(points[0], 1 - points[1]) * (bb.Max - bb.Min) + bb.Min;
		ImVec2 p2 = ImVec2(points[2], 1 - points[3]) * (bb.Max - bb.Min) + bb.Min;
		DrawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), LINE_WIDTH);
		DrawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), LINE_WIDTH);
		DrawList->AddCircleFilled(p1, GRAB_RADIUS, ImColor(white));
		DrawList->AddCircleFilled(p1, GRAB_RADIUS - GRAB_BORDER, ImColor(pink));
		DrawList->AddCircleFilled(p2, GRAB_RADIUS, ImColor(white));
		DrawList->AddCircleFilled(p2, GRAB_RADIUS - GRAB_BORDER, ImColor(cyan));
	}

	 void ShowUI() {
		
		DrawTransferFunction("Transfer Function", this->points.data());
	}
};