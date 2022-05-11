#include "imgui_uie.h"

namespace ImGui
{
	ImVec4 UIEColors[8] =
	{
		ImVec4(0.0f, 0.0f, 0.0f, 1.0f),
		ImVec4(1.0f, 0.361f, 0.361f, 1.0f),
		ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
		ImVec4(0.902f, 0.784f, 0.098f, 1.0f),
		ImVec4(0.0f, 0.0f, 1.0f, 1.0f),
		ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
		ImVec4(1.0f, 0.361f, 1.0f, 1.0f),
		ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	void StyleUIE()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		// Rounding
		style->WindowRounding = 6.0f;
		style->ChildRounding = 0.0f;
		style->FrameRounding = 3.0f;
		style->PopupRounding = 0.0f;
		style->ScrollbarRounding = 2.0f;
		style->GrabRounding = 3.0f;
		style->TabRounding = 2.0f;

		// 
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	ImVec4 GetVec4(float* vector)
	{
		return ImVec4(vector[0], vector[1], vector[2], vector[3]);
	}

	ImVec4 GetColorForText(int index)
	{
		return UIEColors[index];
	}

	void TextCentered(std::string text)
	{
		float font_size = ImGui::GetFontSize() * (text.size() / 2.0f);
		ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));
		ImGui::Text(text.c_str());
	}

	void ColoredText(const char* text, float* col)
	{
		TextColored(GetVec4(col), text);
	}

	IMGUI_API ImVec2 GetButtonSize(std::string button_text)
	{
		return (ImGui::CalcTextSize(button_text.c_str()) + ImGui::GetStyle().FramePadding * 2.0f);
	}
}