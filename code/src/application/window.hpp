#pragma once

namespace uie
{
	class window
	{
	public:
		window(const std::string& name, const WNDPROC& wnd_proc);

		HWND handle_;
	private:
		WNDCLASSEXA wc_;
	};
}