#include <stdafx.hpp>
#include "application.hpp"
#include "renderer/dx11.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"

#include "version.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace uie
{
	LRESULT WINAPI wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		{
			return true;
		}

		switch (msg)
		{
		case WM_SIZE:
			if (wparam == SIZE_MINIMIZED)
			{
				// hello
			}
			else
			{
				renderer::resize(static_cast<std::uint32_t>(LOWORD(lparam)), static_cast<std::uint32_t>(HIWORD(lparam)));
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wparam & 0xFFF0) == SC_KEYMENU) // Disable ALT application menu
			{
				return 0;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}

	application::application(const std::string& name)
	{
		window_ = std::make_unique<window>(name, wnd_proc);

		renderer::init(window_->handle_);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(window_->handle_);
		ImGui_ImplDX11_Init(dx11::device_, dx11::device_context_);
	}

	application::~application()
	{
		renderer::shutdown();
	}

	void application::run()
	{
		bool done = false;
		while (!done)
		{
			MSG msg;
			while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
				
				if (msg.message == WM_QUIT)
				{
					done = true;
				}
			}

			if (done)
			{
				break;
			}

			renderer::begin_frame();

			if (ImGui::Begin("Hello World"))
			{
				ImGui::Text("UIEditor - Revision %i", REVISION_NUMBER);

				static auto white = texture::create("textures/white.png");
				if (white)
				{
					ImGui::ImageButton(white->get_data(), ImVec2(100.0f, 100.0f));
				}
			}
			
			ImGui::End();

			renderer::end_frame();
		}
	}
}