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
#include "misc/filedialog/ImGuiFileDialog.h"

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

		// file dialog
		{
			auto file_dialog = ImGuiFileDialog::Instance();

			file_dialog->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(1.0f, 1.0f, 1.0f, 0.9f), convert_u8_string(ICON_IGFD_FILE_PIC)); // add an icon for the filter type
			file_dialog->SetFileStyle(IGFD_FileStyleByExtention, ".uip", ImVec4(1.0f, 1.0f, 1.0f, 0.9f), convert_u8_string(ICON_IGFD_FILE));
			file_dialog->SetFileStyle(IGFD_FileStyleByExtention, ".uiw", ImVec4(1.0f, 1.0f, 1.0f, 0.9f), convert_u8_string(ICON_IGFD_FILE));
			file_dialog->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(1.0f, 1.0f, 1.0f, 0.9f), convert_u8_string(ICON_IGFD_FOLDER)); // for all dirs
			file_dialog->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1.0f, 1.0f, 1.0f, 0.9f), convert_u8_string(ICON_IGFD_FILE));

			switch (file_dialog_mode_)
			{
			case FILE_DIALOG_SAVE:
				file_dialog->OpenModal("SaveProjectDlg", "Save Project", "UI Project File (*.uip){.uip}", "uieditor\\projects\\", 1, NULL, ImGuiFileDialogFlags_ConfirmOverwrite);
				break;
			case FILE_DIALOG_OPEN:
				file_dialog->OpenModal("OpenProjectDlg", "Open Project", "UI Project File (*.uip){.uip}", "uieditor\\projects\\");
				break;
			case FILE_DIALOG_IMAGE:
				file_dialog->OpenModal("ChooseImageDlg", "Choose Image", ".png", "uieditor\\assets\\images\\");
				break;
			case FILE_DIALOG_BACKGROUND_IMAGE:
				file_dialog->OpenModal("ChooseImageDlg", "Choose Background Image", ".png", "uieditor\\assets\\images\\");
				break;
			}

			if (file_dialog_mode_ != FILE_DIALOG_NONE)
			{
				auto center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(ImVec2(800.0f, 350.0f), ImGuiCond_Appearing);
			
				if (file_dialog->Display("SaveProjectDlg", ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
				{
					if (file_dialog->IsOk())
					{
						project::save_project(ImGuiFileDialog::Instance()->GetCurrentFileName());
					}

					file_dialog_mode_ = FILE_DIALOG_NONE;

					file_dialog->Close();
				}

				if (file_dialog->Display("OpenProjectDlg", ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
				{
					if (file_dialog->IsOk())
					{
						project::load_project(ImGuiFileDialog::Instance()->GetCurrentFileName());
					}

					file_dialog_mode_ = FILE_DIALOG_NONE;

					file_dialog->Close();
				}

				if (file_dialog->Display("ChooseImageDlg", ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
				{
					if (file_dialog->IsOk())
					{
						auto filename = file_dialog->GetCurrentFileName();
						auto filepath = file_dialog->GetFilePathName();

						filepath.erase(0, filepath.find(file_dialog->GetCurrentDialogPath()));

						auto* image = renderer::image::register_handle(filename, filepath);
						if (image)
						{
							if (file_dialog_mode_ == FILE_DIALOG_IMAGE)
							{
								if (properties::element_ != nullptr && image)
								{
									properties::element_->currentAnimationState.image = image;
								}
							}
							else
							{
								canvas::background_image_ = image;
							}
						}
					}

					file_dialog_mode_ = FILE_DIALOG_NONE;

					file_dialog->Close();
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