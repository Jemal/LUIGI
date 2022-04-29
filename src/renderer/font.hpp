#pragma once

namespace renderer
{
	struct font_t
	{
		std::string name;
		int size;
		ImFont* handle;
	};

	class font
	{
	public:
		static std::vector<font_t> fonts_;

		static void register_default_font();

		static void register_font(std::string name, std::string path, int height);

		static ImVec2 get_text_size(const char* text, renderer::font_t* font, float font_height, float wrap_width);
	
	private:
		static std::string default_font;
	};
}