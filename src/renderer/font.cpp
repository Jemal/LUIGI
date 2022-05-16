#include <stdafx.hpp>
#include "engine.hpp"
#include "font.hpp"
#include "uieditor/log.hpp"
#include "uieditor/settings.hpp"
#include "misc/imgui_uie_font.h"

namespace renderer
{
	std::vector< std::unique_ptr<font_t>> font::fonts_;

	ImVec2 font::get_text_size(const char* text, renderer::font_t* font, float font_height, float wrap_width)
	{
		if (text == NULL || font->handle == NULL)
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
		auto& io = ImGui::GetIO();

		ImFontConfig cfg;
		cfg.SizePixels = 18.0f;
		//cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;

		io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(opensans_semibold_compressed_data, opensans_semibold_compressed_size, 18.0f, &cfg);

		// add our icons to the default font
		static const ImWchar icons_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };

		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;

		io.Fonts->AddFontFromMemoryCompressedBase85TTF(ForkAwesome_compressed_data_base85, 15.0f, &icons_config, icons_ranges);
	}

	void font::register_font(std::string name, std::string path, int size)
	{
		font_t font;
		font.name = name;

		if (!utils::io::file_exists(path))
		{
			uieditor::log::print(uieditor::log_error, "Failed to register font '%s' (file not found)", path.data());
			return;
		}

		font.path = path;
		font.size = size;

		ImFontConfig cfg;
		cfg.SizePixels = size;
		cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_NoHinting;

		font.handle = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.data(), size, &cfg);

		fonts_.push_back(std::make_unique<font_t>(font));
	}

	font_t* font::get_font_handle(std::string name)
	{
		for (auto i = 0; i < fonts_.size(); i++)
		{
			auto font = fonts_.at(i).get();
			if (font->name == name)
			{
				return font;
			}
		}

		return nullptr;
	}
}