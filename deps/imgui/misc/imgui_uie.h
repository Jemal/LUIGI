#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include "imgui_internal.h"

#include <string>

namespace ImGui
{
	IMGUI_API void StyleUIE();

	IMGUI_API ImVec4 GetVec4(float* vector);
	IMGUI_API ImVec4 GetColorForText(int index);

	IMGUI_API void TextCentered(std::string text);

	IMGUI_API void ColoredText(const char* text, float* col);
	
	IMGUI_API ImVec2 GetButtonSize(std::string button_text);
}

//IMGUI_API ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
//IMGUI_API ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a);