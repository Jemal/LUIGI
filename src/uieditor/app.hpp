#pragma once

namespace uieditor
{
	enum AppState
	{
		APP_DEFAULT,
		APP_CLOSING,
		APP_SHUTDOWN_AFTER_SAVE,
		APP_SHUTDOWN
	};

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
		
		static bool check_quit();

		static void frame();

		static HWND hwnd_;
		static WNDCLASSEX wc_;

		static bool show_background_;

		static bool show_grid_;
		static float grid_step_;

		static FileDialogMode file_dialog_mode_;

		static ImGuiFileBrowser file_browser_;

		static bool added_font_;

	private:
		static AppState state_;

		static bool show_imgui_demo_;

		static void menubar();
		static void file_browser();
		static void handle_shortcuts();
	};
}