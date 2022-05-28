#include <stdafx.hpp>
#include "application.hpp"
#include "renderer/renderer.hpp"

#include "version.hpp"

#include "backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace uie
{
	application* application::instance_ = nullptr;

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
		instance_ = this;

		window_ = std::make_unique<window>(name, wnd_proc);

		renderer::init(window_->handle_);

		imgui_layer_ = new imgui_layer();
		push_overlay(imgui_layer_);
	}

	application::~application()
	{
		renderer::shutdown();
	}

	void application::push_layer(layer* layer)
	{
		layer_stack_.push_layer(layer);
		layer->on_attach();
	}

	void application::push_overlay(layer* layer)
	{
		layer_stack_.push_overlay(layer);
		layer->on_attach();
	}

	void application::run()
	{
		while (running_)
		{
			MSG msg;
			while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
				
				if (msg.message == WM_QUIT)
				{
					running_ = false;
				}
			}

			if (!running_)
			{
				break;
			}

			if (!minimized_)
			{
				for (auto* layer : layer_stack_)
				{
					layer->on_update();
				}

				imgui_layer_->begin();

				for (auto* layer : layer_stack_)
				{
					layer->on_imgui_render();
				}

				imgui_layer_->end();
			}
		}
	}
}