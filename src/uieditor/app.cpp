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

		project::set_project_name("Untitled", true);

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
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}

	void app::frame()
	{
		menu_bar();

		ImGui::DockSpaceOverViewport();

		if (show_imgui_demo_)
		{
			ImGui::ShowDemoWindow();
		}

		canvas::draw();

		log::draw();

		properties::draw();

		tree::draw();

		// file browser
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
	}

	void app::menu_bar()
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

				if (ImGui::MenuItem("Save As"))
				{
					file_dialog_mode_ = FILE_DIALOG_SAVE;
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
				if (ImGui::BeginMenu("Grid Step"))
				{
					ImGui::InputFloat("##GridStep", &grid_step_, 1.0f, 2.0f, "%.0f");

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("ImGui Demo", NULL, &show_imgui_demo_);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}