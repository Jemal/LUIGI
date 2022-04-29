#pragma once

namespace renderer
{
	struct globals_t
	{
		ID3D11Device* device;
		ID3D11DeviceContext* device_context;
		IDXGISwapChain* swapchain;
		ID3D11RenderTargetView* render_target_view;
		ImVec4 clear_color;
	};

	class engine
	{
	public:
		static void init();
		static void shutdown();
		static void frame();

		static void resize(unsigned int width, unsigned int height);

		static globals_t globals_;

	private:
		static bool create_device(HWND hWnd);
		static void cleanup_device();

		static void create_render_target();
		static void cleanup_render_target();
	};
}