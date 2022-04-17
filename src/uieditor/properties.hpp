#pragma once

namespace uieditor
{
	class properties
	{
	public:
		static void draw();

		static UIElement* element;
		static char element_name[32];
		static char element_text[256];

	private:
		static float input_fast_step;

		static void begin_property(const char* label);
		
		static bool bool_property(const char* label, bool* value);
		static bool input_property(const char* label, ImGuiDataType_ type, void* data, float step, float fast_step);
		static bool slider_property(const char* label, ImGuiDataType_ type, void* data, float min, float max);
		static bool combo_property(const char* label, const char* preview_value);
		static bool color_property(const char* label, float* color);
		static bool text_property(const char* label, char* buf, size_t buf_size);

		static void draw_image_properties();
		static void draw_text_properties();

		static void draw_canvas_properties();
		static void draw_element_properties();
	};
}