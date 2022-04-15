#include <stdafx.hpp>
#include "engine.hpp"
#include "font.hpp"
#include "uieditor/rc/resource.hpp"
#include "uieditor/log.hpp"

namespace renderer
{
	std::string font::default_font;
	std::vector<font_t> font::loaded_fonts;

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
				cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;

				font.handle = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(default_font.data(), default_font.size(), 16.0f, &cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
			}
		}

		loaded_fonts.push_back(font);
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
		cfg.SizePixels = font.size;
		cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;

		font.handle = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath, font.size, &cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

		uieditor::log::print(uieditor::log_message_type::log_normal, "Registered font '%s'", font.name.data());

		loaded_fonts.push_back(font);
	}
}