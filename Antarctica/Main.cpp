#include "stdafx.h"

#include "Core/Application.h"

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		Application app;
		app.Start();
	}
	catch (std::exception e)
	{

		throw;
	}
}
