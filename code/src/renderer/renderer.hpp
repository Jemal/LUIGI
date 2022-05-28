#pragma once

namespace uie
{
	class renderer
	{
	public:
		static void init(HWND& hwnd);
		static void shutdown();

		static void resize(std::uint32_t width, std::uint32_t height);

		static void begin_frame();
		static void end_frame();

		static ID3D11Device* device_;
		static ID3D11DeviceContext* device_context_;
		static IDXGISwapChain* swapchain_;
		static ID3D11RenderTargetView* render_target_;

	private:

	};
}