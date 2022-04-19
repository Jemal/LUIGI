#include <stdafx.hpp>
#include "uieditor/app.hpp"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef DEBUG
	AllocConsole();

	FILE* file = nullptr;
	freopen_s(&file, "CONIN$", "r", stdin);
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stderr);
#endif

	return uieditor::app::init();
}
