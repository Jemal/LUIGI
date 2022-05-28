#pragma once

namespace uie
{
	struct VERTEX_CONSTANT_BUFFER
	{
		float mvp[4][4];
	};

	struct draw_vertex
	{
		float pos[2];
		float uv[2];
		std::uint32_t col;
	};

	typedef unsigned short draw_idx;

	class dx11
	{
	public:
		static void init(HWND& hwnd);
		static void shutdown();

		static ID3D11DeviceContext* device_context_;
		static ID3D11Device* device_;
		static IDXGISwapChain* swapchain_;
		static ID3D11RenderTargetView* render_target_;

	private:
		static ID3D11InputLayout* input_layout_;
		static ID3D11Buffer* vertex_constant_buffer;

		static ID3D11RasterizerState* rasterizer_state_;
		static ID3D11BlendState* blend_state_;
		static ID3D11DepthStencilState* depth_stencil_state_;

		static ID3D11VertexShader* vs_;
		static ID3D11PixelShader* ps_;

		static void create_device_objects();
	};
}