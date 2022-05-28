#include <stdafx.hpp>
#include "renderer.hpp"

namespace uie
{
	ID3D11DeviceContext* renderer::device_context_ = nullptr;
	ID3D11Device* renderer::device_ = nullptr;
	IDXGISwapChain* renderer::swapchain_ = nullptr;
	ID3D11RenderTargetView* renderer::render_target_ = nullptr;

	void renderer::init(HWND& hwnd)
	{
		// initialize swapchain
		DXGI_SWAP_CHAIN_DESC swapchain_desc;
		ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		swapchain_desc.BufferCount = 1;
		swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_desc.OutputWindow = hwnd;
		swapchain_desc.SampleDesc.Count = 1;
		swapchain_desc.Windowed = TRUE;

		// create swapchain, device, and device context
		D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapchain_desc, &swapchain_, &device_, NULL, &device_context_);

		// retrieve backbuffer address 
		ID3D11Texture2D* back_buffer;
		swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
		device_->CreateRenderTargetView(back_buffer, NULL, &render_target_);
		back_buffer->Release();
	}

	void renderer::shutdown()
	{
		swapchain_->Release();
		render_target_->Release();
		device_context_->Release();
		device_->Release();
	}

	void renderer::resize(std::uint32_t width, std::uint32_t height)
	{
		if (device_)
		{
			// clean the render target
			if (render_target_)
			{
				render_target_->Release();
			}

			// resize buffers
			swapchain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

			// create render target
			ID3D11Texture2D* back_buffer;
			swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

			device_->CreateRenderTargetView(back_buffer, NULL, &render_target_);

			back_buffer->Release();
		}
	}

	void renderer::begin_frame()
	{
		/*ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();*/
	}

	void renderer::end_frame()
	{
		/*ImGui::Render();

		dx11::device_context_->OMSetRenderTargets(1, &dx11::render_target_, NULL);

		static float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
		dx11::device_context_->ClearRenderTargetView(dx11::render_target_, clear_color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		dx11::swapchain_->Present(0, 0);*/
	}
}