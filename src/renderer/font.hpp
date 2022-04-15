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
		static std::vector<font_t> loaded_fonts;

		static void register_default_font();

		static void register_font(std::string name, std::string path, int height);
	
	private:
		static std::string default_font;
	};
}