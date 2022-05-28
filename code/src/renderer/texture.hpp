#pragma once

namespace uie
{
	class texture2d
	{
	public:
		texture2d(const std::string& path);

		std::int32_t get_width();
		std::int32_t get_height();

		ID3D11ShaderResourceView* get_data();

	private:
		std::string name;
		std::string path_;
		ID3D11ShaderResourceView* data;
		std::int32_t width_, height_;
	};

	class texture
	{
	public:
		static std::shared_ptr<texture2d> create(const std::string& path);
	};
}