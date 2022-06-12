#pragma once

namespace uie
{
	class properties
	{
	public:
		properties();

		void set_context();

		void render();

		static ui_element* selected_element_;

	private:
		static float input_fast_step_;
		static bool show_color_sets_;

		static void begin_property(const char* label);

		static bool bool_property(const char* label, bool* value);
		static bool input_property(const char* label, ImGuiDataType_ type, void* data, float step, float fast_step);
		static bool slider_property(const char* label, ImGuiDataType_ type, void* data, float min, float max);
		static bool button_property(const char* label, const char* preview_value);
		static bool combo_property(const char* label, const char* preview_value);
		static bool color_property(const char* label, float* color);
		static bool text_property(const char* label, char* buf, size_t buf_size);
		static bool text_property(const char* label, std::string* string);

		static void draw_image_properties();
		static void draw_text_properties();

		static void draw_canvas_properties();
		static void draw_element_properties();
	};
}