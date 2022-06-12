#include <stdafx.hpp>
#include "window.hpp"

#include "editor/resource/resource.hpp"

namespace uie
{
	window::window(const std::string& name, const WNDPROC& wnd_proc)
	{
		memset(&this->wc_, 0, sizeof(WNDCLASSEX));
		this->wc_.cbSize = sizeof(WNDCLASSEX);
		this->wc_.style = CS_CLASSDC;
		this->wc_.lpfnWndProc = wnd_proc;
		this->wc_.cbClsExtra = NULL;
		this->wc_.cbWndExtra = NULL;
		this->wc_.hInstance = GetModuleHandleA(NULL);
		this->wc_.hIcon = LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(ID_ICON));
		this->wc_.hCursor = LoadCursorA(nullptr, IDC_ARROW);
		this->wc_.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		this->wc_.lpszMenuName = nullptr;
		this->wc_.lpszClassName = name.data();
		this->wc_.hIconSm = NULL;

		if (!RegisterClassExA(&this->wc_))
		{
			__debugbreak();
		}

		this->handle_ = CreateWindowA(this->wc_.lpszClassName, name.data(), WS_OVERLAPPEDWINDOW, 100, 100, 1920, 1080, NULL, NULL, this->wc_.hInstance, NULL);

		ShowWindow(this->handle_, SW_SHOWDEFAULT);
		UpdateWindow(this->handle_);
	}
}