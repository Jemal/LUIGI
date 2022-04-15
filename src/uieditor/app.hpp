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

		static HWND hwnd;
		static WNDCLASSEX wc;

	private:
		static bool show_imgui_demo;
	};
}