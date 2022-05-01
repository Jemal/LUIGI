#include <stdafx.hpp>
#include "app.hpp"
#include "canvas.hpp"
#include "log.hpp"
#include "project.hpp"
#include "properties.hpp"
#include "tree.hpp"
#include "rc/resource.hpp"
#include "renderer/engine.hpp"
#include "renderer/image.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace uieditor
{
	bool app::show_background_ = false;

	bool app::show_grid_ = false;
	float app::grid_step_ = 24.0f;

	bool app::show_imgui_demo_ = false;

	bool app::added_font_ = false;
	AppState app::state_ = APP_DEFAULT;

	HWND app::hwnd_;
	WNDCLASSEX app::wc_;

	FileDialogMode app::file_dialog_mode_ = FILE_DIALOG_NONE;

	ImGuiFileBrowser app::file_browser_;

	int app::init()
	{
		ImGui_ImplWin32_EnableDpiAwareness();

		app::create_window();

		renderer::engine::init();

		lui::core::init();

		project::set_project_name("Untitled", false);

		renderer::engine::frame();

		renderer::engine::shutdown();

		return 0;
	}

	void app::create_window()
	{
		wc_.cbSize = sizeof(WNDCLASSEX);
		wc_.style = CS_CLASSDC;
		wc_.lpfnWndProc = uieditor::app::wnd_proc;
		wc_.cbClsExtra = 0;
		wc_.cbWndExtra = 0;
		wc_.hInstance = GetModuleHandle(NULL);
		wc_.hIcon = LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(ID_ICON));
		wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc_.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc_.lpszMenuName = nullptr;
		wc_.lpszClassName = "LUIGI";
		wc_.hIconSm = NULL;

		RegisterClassExA(&wc_);

		hwnd_ = CreateWindowA(wc_.lpszClassName, "LUIGI", WS_OVERLAPPEDWINDOW, 100, 100, 1920, 1080, NULL, NULL, wc_.hInstance, NULL);
	}

	LRESULT WINAPI app::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		{
			return true;
		}

		switch (msg)
		{
		case WM_SIZE:
			if (renderer::engine::globals_.device != NULL && wParam != SIZE_MINIMIZED)
			{
				renderer::engine::resize(static_cast<UINT>(LOWORD(lParam)), static_cast<UINT>(HIWORD(lParam)));
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xFFF0) == SC_KEYMENU) // Disable ALT application menu
			{
				return 0;
			}
			break;
		case WM_CLOSE:
			if (project::state_ == PROJECT_MODIFIED)
			{
				app::state_ = APP_CLOSING;
				return 0;
			}
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		}

		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}

	bool app::check_quit()
	{
		return state_ == APP_SHUTDOWN;
	}

	void app::frame()
	{
		menubar();

		file_browser();

		handle_shortcuts();

		ImGui::DockSpaceOverViewport();

		if (show_imgui_demo_)
		{
			ImGui::ShowDemoWindow();
		}

		canvas::draw();

		log::draw();

		properties::draw();

		tree::draw();

		if (project::show_settings_)
		{
			project::draw_settings();
		}

		if (app::state_ == APP_CLOSING)
		{
			ImGui::OpenPopup("Unsaved Changes");
		}

		ImVec2 window_size(350, 0);
		ImGui::SetNextWindowSize(window_size);
		if (ImGui::BeginPopupModal("Unsaved Changes", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
		{
			ImGui::TextWrapped("Do you want to save the changes you made to %s?", project::project_name_.data());

			ImGui::Separator();

			auto save_size = ImGui::GetButtonSize("Save");
			auto dont_save_size = ImGui::GetButtonSize("Don't Save");
			auto cancel_size = ImGui::GetButtonSize("Cancel");

			float buttons_width = save_size.x + dont_save_size.x + cancel_size.x + ImGui::GetStyle().ItemSpacing.x;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() / 2.0f - buttons_width / 2.0f - ImGui::GetStyle().WindowPadding.x);

			if (ImGui::Button("Save", save_size))
			{
				if (project::state_ == PROJECT_NEW)
				{
					file_dialog_mode_ = FILE_DIALOG_SAVE;
					app::state_ = APP_SHUTDOWN_AFTER_SAVE;
				}
				else
				{
					project::save_project(project::project_name_);
					app::state_ = APP_SHUTDOWN;
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Don't Save", dont_save_size))
			{
				app::state_ = APP_SHUTDOWN;
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", cancel_size))
			{
				app::state_ = APP_DEFAULT;

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void app::menubar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "CTRL+N"))
				{
					project::new_project();
				}

				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					file_dialog_mode_ = FILE_DIALOG_OPEN;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "CTRL+S"))
				{
					if (project::state_ == PROJECT_NEW)
					{
						file_dialog_mode_ = FILE_DIALOG_SAVE;
					}
					else
					{
						project::save_project(project::project_name_);
					}
				}

				if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S"))
				{
					file_dialog_mode_ = FILE_DIALOG_SAVE;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					if (project::state_ == PROJECT_MODIFIED)
					{
						app::state_ = APP_CLOSING;
					}
					else
					{
						app::state_ = APP_SHUTDOWN;
					}

					//added_font_ = true;
					//renderer::font::register_font("HudDigitalExtraBigFont", "iw6\\iw6_digital", 80.0f);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				// maybe one day i'll work on undo/redo

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Show Background Image", "CTRL+B", &show_background_);

				if (ImGui::MenuItem("Background Image"))
				{
					file_dialog_mode_ = FILE_DIALOG_BACKGROUND_IMAGE;
				}

				ImGui::Separator();

				ImGui::MenuItem("Grid", "ALT+G", &show_grid_);

				if (ImGui::MenuItem("Increase Step", "["))
				{
					grid_step_ -= 2.0f;
				}

				if (ImGui::MenuItem("Decrease Step", "]"))
				{
					grid_step_ += 2.0f;
				}

				ImGui::EndMenu();
			}

			ImGui::MenuItem("Settings", NULL, &project::show_settings_);

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("ImGui Demo", NULL, &show_imgui_demo_);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void app::file_browser()
	{
		static bool select_background_image = false;

		switch (file_dialog_mode_)
		{
		case FILE_DIALOG_SAVE:
			file_browser_.open_dialog("Save Project", "./uieditor/projects/");
			file_dialog_mode_ = FILE_DIALOG_NONE;
			break;
		case FILE_DIALOG_OPEN:
			file_browser_.open_dialog("Open Project", "./uieditor/projects/");
			file_dialog_mode_ = FILE_DIALOG_NONE;
			break;
		case FILE_DIALOG_IMAGE:
			file_browser_.open_dialog("Select Image", "./uieditor/assets/images/");
			file_dialog_mode_ = FILE_DIALOG_NONE;
			select_background_image = false;
			break;
		case FILE_DIALOG_BACKGROUND_IMAGE:
			file_browser_.open_dialog("Select Image", "./uieditor/assets/images/");
			file_dialog_mode_ = FILE_DIALOG_NONE;
			select_background_image = true;
			break;
		}

		if (file_browser_.show_dialog("Save Project", ImGuiFileBrowser::DialogMode::SAVE, ImVec2(0, 0), ".uip"))
		{
			project::save_project(file_browser_.selected_fn);

			if (app::state_ == APP_SHUTDOWN_AFTER_SAVE)
			{
				app::state_ = APP_SHUTDOWN;
			}
		}

		if (file_browser_.show_dialog("Open Project", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(0, 0), ".uip"))
		{
			project::load_project(file_browser_.selected_fn);
		}

		if (file_browser_.show_dialog("Select Image", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(0, 0), ".png"))
		{
			auto filename = file_browser_.selected_fn;
			auto filepath = file_browser_.selected_path;;

			filepath.erase(0, filepath.find(file_browser_.current_path));

			auto* image = renderer::image::register_handle(filename, filepath);
			if (image)
			{
				if (select_background_image)
				{
					canvas::background_image_ = image;
				}
				else
				{
					if (properties::element_ != nullptr && image)
					{
						properties::element_->currentAnimationState.image = image;
					}
				}
			}
		}
	}

	void app::handle_shortcuts()
	{
		if (!file_browser_.active)
		{
			if (ImGui::IsKeyDown(ImGuiKey_ModAlt))
			{
				if (ImGui::IsKeyPressed(ImGuiKey_G, false))
				{
					show_grid_ = !show_grid_;
				}
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl))
			{
				if (ImGui::IsKeyPressed(ImGuiKey_N, false))
				{
					project::new_project();
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_O, false))
				{
					file_dialog_mode_ = FILE_DIALOG_OPEN;
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_S, false))
				{
					if (project::state_ == PROJECT_NEW || ImGui::IsKeyDown(ImGuiKey_ModShift))
					{
						file_dialog_mode_ = FILE_DIALOG_SAVE;
					}
					else
					{
						project::save_project(project::project_name_);
					}
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_B, false))
				{
					show_background_ = !show_background_;
				}
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_LeftBracket, false))
			{
				grid_step_ -= 2.0f;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_RightBracket, false))
			{
				grid_step_ += 2.0f;
			}
		}
	}
}