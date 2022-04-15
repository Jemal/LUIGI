#include <stdafx.hpp>
#include "engine.hpp"
#include "image.hpp"
#include "uieditor/log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace renderer
{
	std::vector<image_t> image::loaded_images;

	ID3D11ShaderResourceView* image::create_texture(std::string name)
	{
		int width = 0;
		int height = 0;

		auto* image_data = stbi_load(name.data(), &width, &height, NULL, 4);
		if (!image_data)
		{
			uieditor::log::print(uieditor::log_message_type::log_error, "Failed to load image: %s\n", name.data());
			return nullptr;
		}

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* texture = NULL;
		D3D11_SUBRESOURCE_DATA sub_resource;
		sub_resource.pSysMem = image_data;
		sub_resource.SysMemPitch = desc.Width * 4;
		sub_resource.SysMemSlicePitch = 0;
		engine::globals.device->CreateTexture2D(&desc, &sub_resource, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC texture_view_desc;
		ZeroMemory(&texture_view_desc, sizeof(texture_view_desc));
		texture_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		texture_view_desc.Texture2D.MipLevels = desc.MipLevels;
		texture_view_desc.Texture2D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* texture_view = nullptr;

		if (texture != NULL)
		{
			engine::globals.device->CreateShaderResourceView(texture, &texture_view_desc, &texture_view);
		}

		texture->Release();

		stbi_image_free(image_data);

		return texture_view;
	}

	void image::create_texture_for_image(std::string name, image_t* image)
	{
		auto* image_data = stbi_load(name.data(), &image->width, &image->height, NULL, 4);
		if (!image_data)
		{
			uieditor::log::print(uieditor::log_message_type::log_error, "Failed to load image: %s\n", name.data());
			return;
		}

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = image->width;
		desc.Height = image->height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* texture = NULL;
		D3D11_SUBRESOURCE_DATA sub_resource;
		sub_resource.pSysMem = image_data;
		sub_resource.SysMemPitch = desc.Width * 4;
		sub_resource.SysMemSlicePitch = 0;
		engine::globals.device->CreateTexture2D(&desc, &sub_resource, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC texture_view;
		ZeroMemory(&texture_view, sizeof(texture_view));
		texture_view.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_view.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		texture_view.Texture2D.MipLevels = desc.MipLevels;
		texture_view.Texture2D.MostDetailedMip = 0;

		if (texture != NULL)
		{
			engine::globals.device->CreateShaderResourceView(texture, &texture_view, &image->texture);
		}

		texture->Release();

		stbi_image_free(image_data);
	}

	void image::register_image(std::string name)
	{
		image_t image;

		image.name = name.substr(strlen("uieditor/images/"));
		image.name.erase(image.name.find_last_of("."), 4);
		
		create_texture_for_image(name, &image);

		uieditor::log::print(uieditor::log_message_type::log_normal, "Registered image '%s'", image.name.data());
		
		loaded_images.push_back(image);
	}
}