#pragma once

namespace uieditor
{
	class canvas
	{
	public:
		static void draw();

		static bool show_element_highlight;
		static bool show_grid;
		static float grid_step;
		static bool show_background;

		static renderer::image_t* background;

		static ImVec2 region_min;
		static ImVec2 region_max;
		static ImVec2 size;
		static ImVec2 mouse_pos;

		static void push_stencil(float left, float top, float right, float bottom);
		static void pop_stencil();

		static void draw_image(ID3D11ShaderResourceView* texture, float x, float y, float w, float h, float angle, ImVec4 color);
		static void draw_text(float x, float y, float red, float green, float blue, float alpha, const char* text, renderer::font_t* font, float scale, float wrap_width, int alignment);
	
	private:
		static ImDrawList* draw_list;

		static void draw_grid();
		static void highlight_selected_element(UIElement* element);

		static bool clicked_in_element;
		static int click_mode;
		static int hover_mode;

		static void update_mode_for_anchors(int* mode, UIElement* element, ImVec2 mouse_pos);

		static bool clicked_in_children_bounds(UIElement* element, ImVec2 mouse_pos);
		static bool clicked_in_element_bounds(UIElement* element, ImVec2 mouse_pos);
	};
}