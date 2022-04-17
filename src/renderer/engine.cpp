#include <stdafx.hpp>
#include "engine.hpp"
#include "uieditor/app.hpp"

namespace renderer
{
	globals_t engine::globals = {};

	void engine::create_render_target()
	{
		ID3D11Texture2D* back_buffer;
		globals.swapchain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

		if (back_buffer != NULL)
		{
			globals.device->CreateRenderTargetView(back_buffer, NULL, &globals.render_target_view);
		}

		back_buffer->Release();
	}

	void engine::cleanup_render_target()
	{
		if (globals.render_target_view)
		{
			globals.render_target_view->Release();
			globals.render_target_view = NULL;
		}
	}

	bool engine::create_device(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL featureLevel;
		D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &globals.swapchain, &globals.device, &featureLevel, &globals.device_context) != S_OK)
		{
			return false;
		}

		create_render_target();

		return true;
	}

	void engine::cleanup_device()
	{
		cleanup_render_target();

		if (globals.swapchain)
		{ 
			globals.swapchain->Release();
			globals.swapchain = NULL;
		}

		if (globals.device_context)
		{
			globals.device_context->Release();
			globals.device_context = NULL;
		}
		
		if (globals.device)
		{
			globals.device->Release();
			globals.device = NULL;
		}
	}

	void engine::register_images_for_directory(std::string dir)
	{
		auto list = utils::io::list_files(dir);

		for (auto& file : list)
		{
			if (utils::io::directory_exists(file))
			{
				register_images_for_directory(file);
			}
			else
			{
				image::register_image(file.data());
			}
		}
	}

	void engine::init()
	{
		if (!create_device(uieditor::app::hwnd))
		{
			cleanup_device();
			UnregisterClassA(uieditor::app::wc.lpszClassName, uieditor::app::wc.hInstance);

			return;
		}

		ShowWindow(uieditor::app::hwnd, SW_SHOWDEFAULT);
		UpdateWindow(uieditor::app::hwnd);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		auto* io = &ImGui::GetIO();

		io->IniFilename = "uieditor\\config.ini";

		io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleUIE();

		font::register_default_font();

		// register fonts here or else death
		// need a better way to register fonts since hardcoding them is dumb
		//font::register_font("DebugFont", "consolas", 20.0f);

		ImGui_ImplWin32_Init(uieditor::app::hwnd);
		ImGui_ImplDX11_Init(globals.device, globals.device_context);
		ImGui_ImplDX11_CreateDeviceObjects();

		globals.clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		globals.link_icon = image::create_texture("uieditor\\assets\\link_image.png");

		register_images_for_directory("uieditor\\images");
	}

	void engine::shutdown()
	{
		// crashes when attempting to free our custom fonts
		//ImGui::DestroyContext();

		// force save current settings, this is normally done in DestroyContext
		ImGui::SaveIniSettingsToDisk("uieditor\\config.ini");
		
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();

		cleanup_device();

		DestroyWindow(uieditor::app::hwnd);
		UnregisterClassA(uieditor::app::wc.lpszClassName, uieditor::app::wc.hInstance);
	}

	void engine::frame()
	{
		bool done = false;
		while (!done)
		{
			MSG msg;
			while (PeekMessageA(&msg, NULL, 0U, 0U, PM_REMOVE))
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

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			uieditor::app::frame();

			ImGui::Render();

			globals.device_context->OMSetRenderTargets(1, &globals.render_target_view, NULL);

			const float clear_color_with_alpha[4] = { globals.clear_color.x * globals.clear_color.w, globals.clear_color.y * globals.clear_color.w, globals.clear_color.z * globals.clear_color.w, globals.clear_color.w };
			globals.device_context->ClearRenderTargetView(globals.render_target_view, clear_color_with_alpha);

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			globals.swapchain->Present(1, 0); // Present with vsync
		}
	}

	void engine::resize(unsigned int width, unsigned int height)
	{
		renderer::engine::cleanup_render_target();

		renderer::engine::globals.swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

		renderer::engine::create_render_target();
	}
}