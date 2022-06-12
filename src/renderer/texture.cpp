#include <stdafx.hpp>
#include "renderer.hpp"
#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace uie
{
	namespace
	{
		bool load_texture_from_file(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
		{
			// Load from disk into a raw RGBA buffer
			int image_width = 0;
			int image_height = 0;
			unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
			if (image_data == NULL)
			{
				return false;
			}

			// Create texture
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = image_width;
			desc.Height = image_height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;

			ID3D11Texture2D* pTexture = NULL;
			D3D11_SUBRESOURCE_DATA subResource;
			subResource.pSysMem = image_data;
			subResource.SysMemPitch = desc.Width * 4;
			subResource.SysMemSlicePitch = 0;
			renderer::device_->CreateTexture2D(&desc, &subResource, &pTexture);

			// Create texture view
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			renderer::device_->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
			pTexture->Release();

			*out_width = image_width;
			*out_height = image_height;

			stbi_image_free(image_data);

			return true;
		}
	}

	texture2d::texture2d(const std::string& path)
	{
		if (!load_texture_from_file(path.data(), &this->data, &this->width_, &this->height_))
		{
			printf("failed to load texture '%s' from file\n", path.data());
		}
	}

	std::int32_t texture2d::get_width()
	{
		return this->width_;
	}

	std::int32_t texture2d::get_height()
	{
		return this->height_;
	}

	ID3D11ShaderResourceView* texture2d::get_data()
	{
		return this->data;
	}

	std::shared_ptr<texture2d> texture::create(const std::string& path)
	{
		return std::make_shared<texture2d>(path);
	}
}