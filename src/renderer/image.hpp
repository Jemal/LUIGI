#pragma once

namespace renderer
{
	struct image_t
	{
		std::string name;
		std::string path;
		int width;
		int height;
		ID3D11ShaderResourceView* texture;
	};

	class image
	{
	public:
		static std::vector<std::unique_ptr<image_t>> images_;

		static image_t* register_handle(std::string name, std::string path);
	
	private:
		static ID3D11ShaderResourceView* create_texture(std::string path, int* width, int* height);

	};
}