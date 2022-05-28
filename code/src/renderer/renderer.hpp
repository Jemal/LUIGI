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

	private:

	};
}