#pragma once

namespace uieditor
{
	enum FileDialogMode
	{
		FILE_DIALOG_NONE,
		FILE_DIALOG_SAVE,
		FILE_DIALOG_OPEN,
		FILE_DIALOG_IMAGE,
		FILE_DIALOG_BACKGROUND_IMAGE,
	};

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

		static FileDialogMode file_dialog_mode_;

	private:
		static bool show_imgui_demo_;

		static void menu_bar();
	};
}