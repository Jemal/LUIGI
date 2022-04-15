#pragma once

namespace renderer
{
	struct image_t
	{
		std::string name;
		int width;
		int height;
		ID3D11ShaderResourceView* texture;
	};

	class image
	{
	public:
		static std::vector<image_t> loaded_images;

		static ID3D11ShaderResourceView* create_texture(std::string name);
		static void create_texture_for_image(std::string name, image_t* image);

		static void register_image(std::string name);
	
	private:

	};
}