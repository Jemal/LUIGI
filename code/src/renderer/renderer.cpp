#include <stdafx.hpp>
#include "dx11.hpp"
#include "renderer.hpp"

namespace uie
{
	void renderer::init(HWND& hwnd)
	{
		printf("%s\n", __FUNCTION__);

		dx11::init(hwnd);
	}

	void renderer::shutdown()
	{
		printf("%s\n", __FUNCTION__);

		dx11::shutdown();
	}

	void renderer::resize(std::uint32_t width, std::uint32_t height)
	{
		if (dx11::device_)
		{
			// clean the render target
			if (dx11::render_target_)
			{
				dx11::render_target_->Release();
			}

			// resize buffers
			dx11::swapchain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

			// create render target
			ID3D11Texture2D* back_buffer;
			dx11::swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

			dx11::device_->CreateRenderTargetView(back_buffer, NULL, &dx11::render_target_);

			back_buffer->Release();
		}
	}

	void renderer::begin_frame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void renderer::end_frame()
	{
		ImGui::Render();

		dx11::device_context_->OMSetRenderTargets(1, &dx11::render_target_, NULL);

		static float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
		dx11::device_context_->ClearRenderTargetView(dx11::render_target_, clear_color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		dx11::swapchain_->Present(0, 0);
	}
}