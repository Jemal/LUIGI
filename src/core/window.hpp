#pragma once

namespace uie
{
	class window
	{
	public:
		window(const std::string& name, const WNDPROC& wnd_proc);

		HWND get_handle() { return handle_; };

	private:
		HWND handle_;
		WNDCLASSEXA wc_;
	};
}