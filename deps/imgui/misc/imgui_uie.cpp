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

        style->AntiAliasedLines = true;
        style->AntiAliasedFill = true;

        // Main
        style->WindowPadding.x = 8.0f;
        style->WindowPadding.y = 5.0f;
        style->FramePadding.x = 4.0f;
        style->FramePadding.y = 3.0f;
        style->CellPadding.x = 4.0f;
        style->CellPadding.y = 4.0f;
        style->ItemSpacing.x = 8.0f;
        style->ItemSpacing.y = 3.0f;
        style->ItemInnerSpacing.x = 4.0f;
        style->ItemInnerSpacing.y = 4.0f;
        style->TouchExtraPadding.x = 0.0f;
        style->TouchExtraPadding.y = 0.0f;
        style->IndentSpacing = 21.0f;
        style->ScrollbarSize = 14.0f;
        style->GrabMinSize = 10.0f;

        // Borders
        style->WindowBorderSize = 1.0f;
        style->ChildBorderSize = 1.0f;
        style->PopupBorderSize = 1.0f;
        style->FrameBorderSize = 0.0f;
        style->TabBorderSize = 0.0f;

        // Rounding
        style->WindowRounding = 6.0f;
        style->ChildRounding = 0.0f;
        style->FrameRounding = 3.0f;
        style->PopupRounding = 0.0f;
        style->ScrollbarRounding = 2.0f;
        style->GrabRounding = 3.0f;
        style->TabRounding = 2.0f;

        // Alignment
        style->WindowTitleAlign.x = 0.0f;
        style->WindowTitleAlign.y = 0.5f;
        style->Alpha = 1.0f;

        colors[ImGuiCol_FrameBg] = ImVec4(0.33f, 0.38f, 0.45f, 0.31f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.98f, 0.64f, 0.26f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.98f, 0.62f, 0.26f, 0.67f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.49f, 0.23f, 0.08f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.98f, 0.52f, 0.26f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.62f, 0.24f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.98f, 0.55f, 0.26f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.98f, 0.45f, 0.26f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.62f, 0.26f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.98f, 0.61f, 0.06f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.49f, 0.40f, 0.08f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.81f, 0.26f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.98f, 0.62f, 0.26f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.49f, 0.10f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.51f, 0.10f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.55f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.98f, 0.74f, 0.26f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.98f, 0.79f, 0.26f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.58f, 0.33f, 0.18f, 0.86f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.98f, 0.69f, 0.26f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.49f, 0.40f, 0.08f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.12f, 0.07f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.42f, 0.29f, 0.14f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.98f, 0.71f, 0.26f, 0.35f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.98f, 0.64f, 0.26f, 1.00f);

        // blue
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.42f, 0.98f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.30f, 0.98f, 0.67f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.24f, 0.49f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.49f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.45f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.47f, 0.98f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.51f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.42f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.32f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.08f, 0.24f, 0.49f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.40f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.37f, 0.98f, 1.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.27f, 0.26f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.44f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.23f, 0.58f, 0.86f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.44f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.10f, 0.08f, 0.49f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.44f, 0.98f, 0.35f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.52f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.24f, 0.75f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.08f, 0.11f, 0.23f, 1.00f);
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
}

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)
{
    return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
}