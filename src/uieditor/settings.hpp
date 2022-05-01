#pragma once

namespace uieditor
{
	class settings
	{
	public:
		static void draw();

		static bool show_settings_;

		static void save_font_settings();
		static void load_font_settings();

		static void add_new_saved_font_entry(std::string path);

	private:
		static bool show_add_font_modal_;
		
		static std::string new_font_name_;

		static std::vector<std::unique_ptr<renderer::font_t>> saved_fonts_;

		static void draw_color_settings();
		static void draw_font_settings();

		static void handle_new_font();
	};
}