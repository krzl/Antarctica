#include "stdafx.h"

// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppParameterNeverUsed
// ReSharper disable CppParameterNamesMismatch
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int show)
// ReSharper restore CppParameterNamesMismatch
// ReSharper restore CppParameterNeverUsed
// ReSharper restore CppParameterMayBeConst
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	const ApplicationInfo applicationInfo = {
		1920,
		1080,
		"Graphics Playground",
		165
	};

	try
	{
		Application app(hInst);
		app.Init(applicationInfo, &renderer);
		app.Run();
	}
	catch (std::exception e)
	{
		MessageBox(0, e.what(), 0, 0);
		throw;
	}
}
