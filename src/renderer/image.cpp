#include <stdafx.hpp>
#include "engine.hpp"
#include "image.hpp"
#include "uieditor/log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace renderer
{
	std::vector<std::unique_ptr<image_t>> image::images_;

	ID3D11ShaderResourceView* image::create_texture(std::string path, int* width, int* height)
	{
		auto* image_data = stbi_load(path.data(), width, height, NULL, 4);
		if (!image_data)
		{
			uieditor::log::print(uieditor::log_message_type::log_error, "Failed to create texture: %s\n", path.data());
			return nullptr;
		}

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = *width;
		desc.Height = *height;
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
		engine::globals_.device->CreateTexture2D(&desc, &sub_resource, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC texture_view_desc;
		ZeroMemory(&texture_view_desc, sizeof(texture_view_desc));
		texture_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		texture_view_desc.Texture2D.MipLevels = desc.MipLevels;
		texture_view_desc.Texture2D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* texture_view = nullptr;

		if (texture != NULL)
		{
			engine::globals_.device->CreateShaderResourceView(texture, &texture_view_desc, &texture_view);
		}

		texture->Release();

		stbi_image_free(image_data);

		return texture_view;
	}

	image_t* image::register_handle(std::string name, std::string path)
	{
		for (auto i = 0; i < images_.size(); i++)
		{
			auto image = images_.at(i).get();
			if (image->name == name)
			{
				return image;
			}
		}

		image_t new_image;
		new_image.name = name;
		new_image.path = path;

		new_image.texture = create_texture(new_image.path, &new_image.width, &new_image.height);
		if (new_image.texture == NULL)
		{
			return nullptr;
		}

		images_.push_back(std::make_unique<image_t>(new_image));

		// not good
		return register_handle(name, path);
	}
}