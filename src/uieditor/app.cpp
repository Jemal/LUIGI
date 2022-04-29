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

	int app::init()
	{
		ImGui_ImplWin32_EnableDpiAwareness();

		app::create_window();

		renderer::engine::init();

		lui::core::init();

		SetWindowTextA(hwnd_, "LUIGI");

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
	}

	void app::menu_bar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("New", "CTRL+N");

				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					project::opening_popup_ = true;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "CTRL+S"))
				{
					if (project::project_name_.empty())
					{
						project::saving_popup_ = true;
					}
					else
					{
						project::save_project(project::project_name_);
					}
				}

				if (ImGui::MenuItem("Save As"))
				{
					project::saving_popup_ = true;
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

				if (ImGui::BeginMenu("Background Image"))
				{
					if (ImGui::BeginCombo("##BackgroundImage", canvas::background_image_ == nullptr ? "Select..." : canvas::background_image_->name.data()))
					{
						for (auto i = 0; i < renderer::image::images_.size(); i++)
						{
							auto* image = renderer::image::images_.at(i).get();

							if (ImGui::Selectable(image->name.data()))
							{
								canvas::background_image_ = image;
							}
						}

						ImGui::EndCombo();
					}

					ImGui::EndMenu();
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
	
		auto file_dialog = ImGuiFileDialog::Instance();

		file_dialog->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f)); // add an icon for the filter type
		file_dialog->SetFileStyle(IGFD_FileStyleByExtention, ".uip", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
		file_dialog->SetFileStyle(IGFD_FileStyleByExtention, ".uiw", ImVec4(0.0f, 1.0f, 0.0f, 0.9f));
		//file_dialog->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER); // for all dirs

		const char* filters = "UI Project File (*.uip){.uip}, UI Widget File (*.uiw){.uiw}";

		if (project::saving_popup_)
		{
			//ImGui::OpenPopup("Save");
			file_dialog->OpenModal("SaveProjectDlg", "Save Project", filters, "uieditor/projects/", 1, IGFDUserDatas("SaveFile"), ImGuiFileDialogFlags_ConfirmOverwrite);
		}

		if (project::opening_popup_)
		{
			file_dialog->OpenModal("OpenProjectDlg", "Open Project", filters, "uieditor/projects/");
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(800.0f, 350.0f), ImGuiCond_Appearing);

		if (file_dialog->Display("SaveProjectDlg", ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
		{
			if (file_dialog->IsOk())
			{
				project::save_project(ImGuiFileDialog::Instance()->GetCurrentFileName());
			}

			project::saving_popup_ = false;

			file_dialog->Close();
		}

		if (file_dialog->Display("OpenProjectDlg", ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
		{
			if (file_dialog->IsOk())
			{
				project::load_project(ImGuiFileDialog::Instance()->GetCurrentFileName());

				SetWindowTextA(app::hwnd_, utils::string::va("LUIGI - %s", project::project_name_.data()));
			}

			project::opening_popup_ = false;

			file_dialog->Close();
		}

		if (ImGui::BeginPopupModal("Save", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::InputText("Project Name", &project::project_name_);

			ImGui::Separator();

			if (ImGui::Button("Save", ImVec2(200, 0)))
			{
				project::project_name_.append(".uip");

				project::save_project(project::project_name_);

				SetWindowTextA(app::hwnd_, utils::string::va("LUIGI - %s", project::project_name_.data()));

				project::saving_popup_ = false;

				ImGui::CloseCurrentPopup();
			}
			
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			
			if (ImGui::Button("Cancel", ImVec2(200, 0)))
			{
				project::saving_popup_ = false;

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}