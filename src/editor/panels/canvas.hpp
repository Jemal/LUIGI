#pragma once

namespace uie
{
	class canvas
	{
	public:
		canvas();

		void set_context();

		void render();

		static ImVec2 get_size();
		
		static void draw_image(ID3D11ShaderResourceView* texture, float x, float y, float w, float h, float angle, ImVec4 color);

	private:
		static ImVec4 region_;
		static ImVec2 size_;

		static ImVec2 mouse_pos_;
		static ImVec2 scroll_pos_;

		//static void draw_text(UIElement* element, float x, float y, float red, float green, float blue, float alpha, const char* text, renderer::font_t* font, float font_height, float wrap_width, int alignment);

		void ui_tab_bar();
	};
}