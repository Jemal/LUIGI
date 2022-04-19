#pragma once

namespace uieditor
{
	class canvas
	{
	public:
		static void draw();

		static float zoom_pct_;

		static renderer::image_t* background_image_;

		static ImVec4 region_;
		static ImVec2 size_;

		static ImVec2 mouse_pos_;

		static void push_stencil(float left, float top, float right, float bottom);
		static void pop_stencil();

		static void draw_image(ID3D11ShaderResourceView* texture, float x, float y, float w, float h, float angle, ImVec4 color);
		static void draw_text(float x, float y, float red, float green, float blue, float alpha, const char* text, renderer::font_t* font, float font_height, float wrap_width, int alignment);
	
	private:
		static ImDrawList* draw_list_;

		static void draw_grid();
		static void highlight_selected_element(UIElement* element, bool hovered);

		static bool clicked_in_element_;
		static bool hovering_element_;

		static int click_mode_;
		static int hover_mode_;

		static UIElement* hovered_element_;

		static bool link_width_height_;

		static void update_mode_for_anchors(int* mode, UIElement* element, ImVec2 mouse_pos);

		static bool clicked_in_children_bounds(UIElement* element, ImVec2 mouse_pos, bool hover);
		static bool clicked_in_element_bounds(UIElement* element, ImVec2 mouse_pos, bool hover);

		static void handle_element_transform(UIElement* element);
	};
}