#include <stdafx.hpp>
#include "engine.hpp"
#include "font.hpp"
#include "uieditor/rc/resource.hpp"
#include "uieditor/log.hpp"
#include "misc/filebrowser/ImGuiFileBrowser.h"
#include "utils/json.hpp"

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

	void font::load_font_settings()
	{
		auto path = "uieditor\\config\\fonts.json";
		if (!utils::io::file_exists(path))
		{
			uieditor::log::print(uieditor::log_message_type::log_error, "Failed to load font settings for project");
			return;
		}

		auto font_settings = utils::io::read_file(path);
		if (!font_settings.empty())
		{
			auto data = nlohmann::json::parse(font_settings);

			for (auto& item : data.items())
			{
				auto values = item.value();

				auto font_name = values["Name"];
				auto font_path = values["Path"];
				auto font_size = values["Size"];

				if (font_name.is_null() || font_path.is_null() || font_size.is_null())
				{
					return;
				}

				font::register_font(font_name, font_path, font_size);
			}
		}
	}

	void font::register_default_font()
	{
		font_t font;
		font.name = "default";
		font.size = 18.0f;

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

				font.handle = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(default_font.data(), default_font.size(), font.size, &cfg);

				static const ImWchar icons_ranges[] = { ICON_MIN_IGFD, ICON_MAX_IGFD, 0 };

				ImFontConfig icons_config;
				icons_config.MergeMode = true; 
				icons_config.PixelSnapH = true;

				ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, 15.0f, &icons_config, icons_ranges);
			}
		}

		fonts_.push_back(font);
	}

	void font::register_font(std::string name, std::string path, int size)
	{
		if (!utils::io::file_exists(path))
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

		font.handle = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.data(), size, &cfg);

		fonts_.push_back(font);
	}
}