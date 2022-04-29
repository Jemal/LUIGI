#include <stdafx.hpp>
#include "engine.hpp"
#include "font.hpp"
#include "uieditor/rc/resource.hpp"
#include "uieditor/log.hpp"

namespace renderer
{
	std::string font::default_font;
	std::vector<font_t> font::fonts_;

	ImVec2 font::get_text_size(const char* text, renderer::font_t* font, float font_height, float wrap_width)
	{
		if (text == NULL)
		{
			return ImVec2(0.0f, 0.0f);
		}

		auto font_scale__ = font_height / font->size;
		auto size = font->handle->Ascent * font_scale__;

		auto text_size = font->handle->CalcTextSizeA(size, FLT_MAX, wrap_width, text, NULL, NULL);

		// round
		text_size.x = ((float)(int)(text_size.x + 0.99999f));

		return text_size;
	}

	void font::register_default_font()
	{
		font_t font;
		font.name = "default";
		font.size = 16.0f;

		auto font_resource = FindResourceA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(ID_DEFAULTFONT), RT_RCDATA);
		if (font_resource)
		{
			auto handle = LoadResource(nullptr, font_resource);
			if (handle)
			{
				default_font = std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, font_resource));

				ImFontConfig cfg;
				cfg.SizePixels = font.size;
				//cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;

				font.handle = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(default_font.data(), default_font.size(), 16.0f, &cfg);

				static const ImWchar icons_ranges[] = { ICON_MIN_IGFD, ICON_MAX_IGFD, 0 };
				ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
				ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, 15.0f, &icons_config, icons_ranges);
			}
		}

		fonts_.push_back(font);
	}

	void font::register_font(std::string name, std::string path, int size)
	{
		auto filepath = utils::string::va("uieditor\\fonts\\%s.ttf", path.data());
		if (!utils::io::file_exists(filepath))
		{
			uieditor::log::print(uieditor::log_message_type::log_error, "Failed to register font '%s' (file not found)", path.data());
			return;
		}

		font_t font;
		font.name = name;
		font.size = size;

		ImFontConfig cfg;
		cfg.SizePixels = size;
		cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_NoHinting;

		font.handle = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath, size, &cfg);

		uieditor::log::print(uieditor::log_normal, "Registered font '%s'", font.name.data());

		fonts_.push_back(font);
	}
}