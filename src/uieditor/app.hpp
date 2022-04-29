#pragma once

namespace uieditor
{
	class app
	{
	public:
		static int init();

		static void create_window();
		static LRESULT WINAPI wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		static void frame();

		static HWND hwnd_;
		static WNDCLASSEX wc_;

		static bool show_background_;

		static bool show_grid_;
		static float grid_step_;

	private:
		static bool show_imgui_demo_;

		static void menu_bar();
	};
}