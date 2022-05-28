#include <stdafx.hpp>
#include "dx11.hpp"

#include "shaders/pixel_shader.hpp"
#include "shaders/vertex_shader.hpp"

#include <d3dcompiler.h>

namespace uie
{
	ID3D11DeviceContext* dx11::device_context_ = nullptr;
	ID3D11Device* dx11::device_ = nullptr;
	IDXGISwapChain* dx11::swapchain_ = nullptr;
	ID3D11RenderTargetView* dx11::render_target_ = nullptr;

	ID3D11Buffer* dx11::vertex_constant_buffer = nullptr;
	ID3D11InputLayout* dx11::input_layout_ = nullptr;

	ID3D11RasterizerState* dx11::rasterizer_state_ = nullptr;
	ID3D11BlendState* dx11::blend_state_ = nullptr;
	ID3D11DepthStencilState* dx11::depth_stencil_state_ = nullptr;

	ID3D11VertexShader* dx11::vs_ = nullptr;
	ID3D11PixelShader* dx11::ps_ = nullptr;

	void dx11::init(HWND& hwnd)
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

		device_->AddRef();
		device_context_->AddRef();

		//create_device_objects();
	}

	void dx11::shutdown()
	{
		swapchain_->Release();
		render_target_->Release();
		device_context_->Release();
		device_->Release();
	}

	void dx11::create_device_objects()
	{
		ID3DBlob* vs_blob;
		if (FAILED(D3DCompile(vertex_shader, strlen(vertex_shader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &vs_blob, NULL)))
		{
			return; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
		}

		if (device_->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &vs_) != S_OK)
		{
			return;
		}

		// Create the input layout
		D3D11_INPUT_ELEMENT_DESC local_layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(draw_vertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(draw_vertex, uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)offsetof(draw_vertex, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (device_->CreateInputLayout(local_layout, 3, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout_) != S_OK)
		{
			vs_blob->Release();
			return;
		}

		vs_blob->Release();

		// create the constant buffer
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		device_->CreateBuffer(&desc, NULL, &vertex_constant_buffer);

		// create the pixel shader
		ID3DBlob* ps_blob;
		if (FAILED(D3DCompile(pixel_shader, strlen(pixel_shader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &ps_blob, NULL)))
		{
			return; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
		}

		if (device_->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), NULL, &ps_) != S_OK)
		{
			return;
		}

		// Create the blending setup
		{
			D3D11_BLEND_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.AlphaToCoverageEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			device_->CreateBlendState(&desc, &blend_state_);
		}

		// Create the rasterizer state
		{
			D3D11_RASTERIZER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_NONE;
			desc.ScissorEnable = true;
			desc.DepthClipEnable = true;
			device_->CreateRasterizerState(&desc, &rasterizer_state_);
		}

		// Create depth-stencil State
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = false;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			desc.StencilEnable = false;
			desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace = desc.FrontFace;
			device_->CreateDepthStencilState(&desc, &depth_stencil_state_);
		}
	}
}