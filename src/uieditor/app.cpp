#include <stdafx.hpp>
#include "app.hpp"
#include "canvas.hpp"
#include "log.hpp"
#include "properties.hpp"
#include "tree.hpp"
#include "rc/resource.hpp"
#include "renderer/engine.hpp"
#include "renderer/image.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace uieditor
{
	bool app::show_imgui_demo = false;

	HWND app::hwnd;
	WNDCLASSEX app::wc;

	int app::init()
	{
		ImGui_ImplWin32_EnableDpiAwareness();

		app::create_window();

		renderer::engine::init();

		lui::core::init();

		SetWindowTextA(hwnd, "LUIGI - Untitled");

		renderer::engine::frame();

		renderer::engine::shutdown();

		return 0;
	}

	void app::create_window()
	{
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_CLASSDC;
		wc.lpfnWndProc = uieditor::app::wnd_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(ID_ICON));
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = "LUIGI";
		wc.hIconSm = NULL;

		RegisterClassExA(&wc);

		hwnd = CreateWindowA(wc.lpszClassName, "LUIGI", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 1920, 1080, NULL, NULL, wc.hInstance, NULL);
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
			if (renderer::engine::globals.device != NULL && wParam != SIZE_MINIMIZED)
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
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("New...");
				ImGui::MenuItem("Open...");
				ImGui::MenuItem("Save...");
				ImGui::MenuItem("Save As...");

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				// maybe one day i'll work on undo/redo

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Canvas"))
			{
				ImGui::MenuItem("Show highlighted element", NULL, &canvas::show_element_highlight);

				if (ImGui::BeginMenu("Grid"))
				{
					ImGui::Checkbox("Show", &canvas::show_grid);

					ImGui::InputFloat("Step", &canvas::grid_step, 1.0f, 2.0f, "%.0f");

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Background"))
				{
					ImGui::Checkbox("Show", &canvas::show_background);

					if (ImGui::BeginCombo("Image", canvas::background == nullptr ? "Select..." : canvas::background->name.data()))
					{
						for (auto i = 0; i < renderer::image::loaded_images.size(); i++)
						{
							auto* image = &renderer::image::loaded_images.at(i);

							if (ImGui::Selectable(image->name.data()))
							{
								canvas::background = image;
							}
						}

						ImGui::EndCombo();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("ImGui Demo", NULL, &show_imgui_demo);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::DockSpaceOverViewport();

		if (show_imgui_demo)
		{
			ImGui::ShowDemoWindow();
		}

		canvas::draw();

		log::draw();

		properties::draw();

		tree::draw();
	}
}